/* Unit tests for libs/AT_Module/AT.c
 *
 * Command handlers that reboot the MCU (+RST, +Z success path, +DEL) end in
 * `while(1)` waiting for the watchdog, so only their observable pre-reboot
 * behaviour is exercised here. See tests/README.md for the handlers that are
 * left untested because they dereference invalid memory.
 */
#include <string.h>

#include "AT.h"
#include "app_task.h"
#include "drv_charger.h"
#include "drv_load.h"
#include "fakes.h"
#include "sys_manager.h"
#include "test_framework.h"
#include "uart.h"

ERR_CODE at_outoput_cb(char *buffer);
ERR_CODE at_charger_cb(char *buffer);
ERR_CODE at_charger_cb_chb(char *buffer);
ERR_CODE at_em_cb(char *buffer);
ERR_CODE at_factory_cb(char *buffer);
ERR_CODE at_sleep_cb(char *buffer);
ERR_CODE at_ver_cb(char *buffer);
ERR_CODE at_vol_get_cb(char *buffer);
ERR_CODE at_open_all_cb(char *buffer);
ERR_CODE at_swt_cb(char *buffer);
ERR_CODE at_st_cb(char *buffer);
ERR_CODE rgb_test(char *buffer);
uint32_t cut_buffer_decimal_number(char *dst, char *src, uint32_t src_len);

static void reset_env(void)
{
    fakes_reset();
    memset((void *)&__sys_manager, 0, sizeof(__sys_manager));
}

static int ble_tx_contains(const char *needle)
{
    uint32_t i;

    for (i = 0; i < fakes.ble_tx_cnt; i++) {
        if (strstr(fakes.ble_tx[i], needle) != NULL) {
            return 1;
        }
    }
    return 0;
}

static int timer_delay(uint16_t timer_id, uint32_t *delay)
{
    uint32_t i;

    for (i = 0; i < fakes.timer_cnt; i++) {
        if (fakes.timers[i].msg_id == timer_id) {
            *delay = fakes.timers[i].delay;
            return 1;
        }
    }
    return 0;
}

/* ---------------- dispensing (+O:) ---------------- */

static void output_dispenses_the_requested_slot(void)
{
    /* The slot digit is read from index 3, i.e. commands arrive as "+O:n". */
    char buffer[] = "+O:3";
    uint32_t delay = 0;

    reset_env();
    uart_rx_done = 1;

    CHECK_EQ_INT(at_outoput_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(__sys_manager.dial_number, 3);
    CHECK_EQ_INT(__sys_manager.output_stat, 1);
    CHECK_EQ_INT(fakes.uart2_tx_cnt, 1);
    CHECK_EQ_INT(fakes.uart2_tx[0], 0xC3); /* position 3 command byte */
    CHECK_EQ_INT(fakes.uart2_last_baudrate, 9600);
    CHECK_EQ_INT(fakes.wdt_feed_cnt, 1);
    CHECK(timer_delay(APP_LOAD_CLOSE_TIMER, &delay));
    CHECK_EQ_INT(delay, 300);
}

static void output_rejects_slot_zero_and_slots_past_the_last_load(void)
{
    char slot_zero[] = "+O:0";
    char slot_too_high[] = "+O:7";

    reset_env();
    uart_rx_done = 1;

    CHECK_EQ_INT(at_outoput_cb(slot_zero), ERR_NONENTITY);
    CHECK_EQ_INT(at_outoput_cb(slot_too_high), ERR_NONENTITY);
    CHECK_EQ_INT(__sys_manager.output_stat, 0);
    CHECK_EQ_INT(fakes.uart2_tx_cnt, 0);
    CHECK_EQ_INT(fakes.timer_cnt, 0);
}

static void output_accepts_the_last_valid_slot(void)
{
    char buffer[] = "+O:6";

    reset_env();
    uart_rx_done = 1;

    CHECK_EQ_INT(at_outoput_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(__sys_manager.dial_number, MAX_LOAD_NUMBER - 1);
    CHECK_EQ_INT(fakes.uart2_tx[0], 0x96);
}

static void output_requires_authorization_when_not_from_uart(void)
{
    char buffer[] = "+O:2";

    reset_env();
    uart_rx_done = 0;
    __sys_manager.authorization = 0;

    CHECK_EQ_INT(at_outoput_cb(buffer), ERR_UNAUTHORIZED);
    CHECK_EQ_INT(fakes.uart2_tx_cnt, 0);
}

static void output_is_rejected_while_a_dispense_is_running(void)
{
    char buffer[] = "+O:2";

    reset_env();
    uart_rx_done = 0;
    __sys_manager.authorization = 1;

    CHECK_EQ_INT(at_outoput_cb(buffer), ERR_DIAL_IS_RUN);

    __sys_manager.output_stat = 1;
    CHECK_EQ_INT(at_outoput_cb(buffer), ERR_UNAUTHORIZED);
    CHECK_EQ_INT(fakes.uart2_tx_cnt, 0);
}

/* ---------------- charging (+CHA / +CHB) ---------------- */

static void charger_arms_the_close_timer_for_the_requested_hours(void)
{
    char buffer[] = "AT+CH2";
    uint32_t delay = 0;

    reset_env();
    __sys_manager.authorization = 1;

    CHECK_EQ_INT(at_charger_cb(buffer), ERR_NONE);
    /* 2 h plus the 60 s slack the firmware adds for software-timer drift. */
    CHECK_EQ_INT(__sys_manager.charger_cnt, 2 * 3600 + 60);
    CHECK_EQ_INT(__sys_manager.charger_en, 1);
    CHECK_EQ_INT(__sys_manager.authorization, 0);
    CHECK_EQ_INT(fake_gpio_level(CHARGER_PW_GPIO), 1);
    CHECK(timer_delay(APP_CHAR_CLOSE_TIMER, &delay));
    CHECK_EQ_INT(delay, (2 * 3600 + 60) * SEC);
    CHECK(ble_tx_contains("ISCHA"));
}

static void charger_clamps_the_request_to_eight_hours(void)
{
    char buffer[] = "AT+CH9";

    reset_env();
    __sys_manager.authorization = 1;

    CHECK_EQ_INT(at_charger_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(__sys_manager.charger_cnt, 8 * 3600 + 60);
}

static void charger_requires_authorization(void)
{
    char buffer[] = "AT+CH2";

    reset_env();
    __sys_manager.authorization = 0;

    CHECK_EQ_INT(at_charger_cb(buffer), ERR_UNAUTHORIZED);
    CHECK_EQ_INT(__sys_manager.charger_en, 0);
    CHECK_EQ_INT(fakes.timer_cnt, 0);
    CHECK_EQ_INT(fakes.ble_tx_cnt, 0);
}

static void charger_seconds_variant_uses_the_decimal_argument(void)
{
    char buffer[] = "AT+CH120";
    uint32_t delay = 0;

    reset_env();
    __sys_manager.authorization = 1;

    CHECK_EQ_INT(at_charger_cb_chb(buffer), ERR_NONE);
    CHECK_EQ_INT(fake_gpio_level(CHARGER_PW_GPIO), 1);
    CHECK(timer_delay(APP_CHAR_CLOSE_TIMER, &delay));
    CHECK_EQ_INT(delay, 120 * SEC);
    CHECK(ble_tx_contains("ISCHA"));
    CHECK_EQ_INT(__sys_manager.authorization, 0);
}

static void charger_seconds_variant_requires_authorization(void)
{
    char buffer[] = "AT+CH120";

    reset_env();

    CHECK_EQ_INT(at_charger_cb_chb(buffer), ERR_UNAUTHORIZED);
    CHECK_EQ_INT(fakes.timer_cnt, 0);
}

/* ---------------- informational handlers ---------------- */

static void version_handler_notifies_the_firmware_version(void)
{
    char buffer[] = "AT+VER";

    reset_env();

    CHECK_EQ_INT(at_ver_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(fakes.ble_tx_cnt, 1);
    CHECK(ble_tx_contains("VER:"));
}

static void voltage_handler_notifies_a_millivolt_reading(void)
{
    char buffer[] = "AT+V?";

    reset_env();
    fakes.adc_value = 620;

    CHECK_EQ_INT(at_vol_get_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(fakes.ble_tx_cnt, 1);
    CHECK_EQ_STR(fakes.ble_tx[0], "V:4031\r\n");
}

static void sleep_handler_drops_the_ble_link(void)
{
    char buffer[] = "AT+S?";

    reset_env();

    CHECK_EQ_INT(at_sleep_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(fakes.disconnect_cnt, 1);
}

static void factory_handler_needs_authorization(void)
{
    char buffer[] = "AT+FAC";

    reset_env();
    CHECK_EQ_INT(at_factory_cb(buffer), ERR_UNAUTHORIZED);

    __sys_manager.authorization = 1;
    CHECK_EQ_INT(at_factory_cb(buffer), ERR_NONE);
}

static void unimplemented_handlers_succeed_without_side_effects(void)
{
    char buffer[] = "AT+ALL";

    reset_env();

    CHECK_EQ_INT(at_em_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(at_open_all_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(at_swt_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(fakes.write_cnt, 0);
    CHECK_EQ_INT(fakes.ble_tx_cnt, 0);
}

/* ---------------- name programming (+Z) ---------------- */

static void cut_decimal_number_keeps_only_digits(void)
{
    char dst[32] = {0};
    char src[] = "AT+Z1234abc56";

    CHECK_EQ_INT(cut_buffer_decimal_number(dst, src, strlen(src)), 6);
    CHECK_EQ_STR(dst, "123456");
}

static void cut_decimal_number_stops_at_the_requested_length(void)
{
    char dst[32] = {0};
    char src[] = "12345678";

    CHECK_EQ_INT(cut_buffer_decimal_number(dst, src, 4), 4);
    CHECK_EQ_STR(dst, "1234");
}

static void cut_decimal_number_returns_zero_without_digits(void)
{
    char dst[8] = {0};
    char src[] = "AT+Z";

    CHECK_EQ_INT(cut_buffer_decimal_number(dst, src, strlen(src)), 0);
    CHECK_EQ_INT(dst[0], 0);
}

static void set_mac_address_rejects_names_that_are_not_16_digits(void)
{
    /* The handler always scans 18 bytes, so the buffer must be that long. */
    char buffer[20] = "AT+Z12345";

    reset_env();

    CHECK_EQ_INT(at_set_mac_addres_cb(buffer), ERR_NAME_LEN);
    CHECK_EQ_INT(__sys_manager.ble_name_len, 5);
    CHECK_EQ_INT(fakes.flash_write_cnt, 0);
    CHECK_EQ_INT(fakes.wdt_enable_cnt, 0);
}

static void set_mac_address_reports_failure_when_flash_does_not_match(void)
{
    /* Only the first 18 bytes are scanned, so a 16-digit name needs at most
     * two non-digits. Here the readback of the digit-only name is compared
     * against `buffer + 2`, which differs, so the write is reported failed. */
    char buffer[] = "+1234567890123456Z";

    reset_env();
    __sys_manager.authorization = 1;

    CHECK_EQ_INT(at_set_mac_addres_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(__sys_manager.ble_name_len, 16);
    CHECK_EQ_INT(fakes.flash_write_cnt, 1);
    CHECK_EQ_INT(fakes.flash_writes[0].address, 0x8080);
    CHECK_EQ_INT(__sys_manager.config.name_flag, 0);
    CHECK(ble_tx_contains("Write failed"));
    CHECK_EQ_INT(fakes.wdt_enable_cnt, 0);
}

static void set_mac_address_is_refused_without_uart_or_authorization(void)
{
    char buffer[] = "+1234567890123456Z";

    reset_env();
    __sys_manager.config.name_flag = 1; /* already provisioned */
    __sys_manager.authorization = 0;
    uart_rx_done = 0;

    CHECK_EQ_INT(at_set_mac_addres_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(fakes.flash_write_cnt, 0);
    CHECK(ble_tx_contains("Write failed"));
}

/* ---------------- rgb / timing handlers ---------------- */

static void rgb_handler_parses_channel_and_hex_colour(void)
{
    /* Channel comes from index 4, the 8 colour characters follow directly. */
    char buffer[] = "RGB:10xFF8800";

    reset_env();

    CHECK_EQ_INT(rgb_test(buffer), ERR_NONE);
    CHECK_EQ_INT(fakes.rgb_set_cnt, 1);
    CHECK_EQ_INT(fakes.rgb_last_ch, 1);
    CHECK_EQ_INT(fakes.rgb_last_color, 0xFF8800);
}

static void rgb_handler_treats_unparsable_colour_as_zero(void)
{
    char buffer[] = "RGB:0zzzzzzzz";

    reset_env();

    CHECK_EQ_INT(rgb_test(buffer), ERR_NONE);
    CHECK_EQ_INT(fakes.rgb_last_ch, 0);
    CHECK_EQ_INT(fakes.rgb_last_color, 0);
}

static void st_handler_stores_the_requested_time(void)
{
    char buffer[] = "+ST:45";

    reset_env();

    CHECK_EQ_INT(at_st_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(__sys_manager.time, 45);
}

static void st_handler_stores_zero_without_a_number(void)
{
    char buffer[] = "+ST:";

    reset_env();
    __sys_manager.time = 99;

    CHECK_EQ_INT(at_st_cb(buffer), ERR_NONE);
    CHECK_EQ_INT(__sys_manager.time, 0);
}

/* ---------------- dispatch table ---------------- */

static void traverse_dispatches_the_matching_handler(void)
{
    char buffer[] = "AT+VER\r\n";

    reset_env();

    CHECK_EQ_INT(at_traverse(buffer, AT_Shell), ERR_NONE);
    CHECK_EQ_INT(__sys_manager.authorization, 1);
    CHECK(ble_tx_contains("VER:"));
}

static void traverse_ignores_unknown_commands(void)
{
    char buffer[] = "AT+NOPE\r\n";

    reset_env();

    CHECK_EQ_INT(at_traverse(buffer, AT_Shell), ERR_NONE);
    CHECK_EQ_INT(fakes.ble_tx_cnt, 0);
    CHECK_EQ_INT(fakes.write_cnt, 0);
}

static void traverse_propagates_handler_error_codes(void)
{
    AT shell[SHELL_LEN];
    char buffer[] = "AT+Z12";
    uint8_t i;

    reset_env();
    for (i = 0; i < SHELL_LEN; i++) {
        shell[i].AT_CMD = "+Z";
        shell[i].cmd_cb = at_set_mac_addres_cb;
    }

    CHECK_EQ_INT(at_traverse(buffer, shell), ERR_NAME_LEN);
}

/* ---------------- error reporting ---------------- */

static void error_report_notifies_and_clears_the_pending_error(void)
{
    reset_env();
    __sys_manager.err_code = ERR_NONENTITY;
    __sys_manager.output_stat = 1;

    error_event_report();

    CHECK_EQ_INT(fakes.ble_tx_cnt, 1);
    CHECK_EQ_STR(fakes.ble_tx[0], "ERR:1010\r\n");
    CHECK_EQ_INT(__sys_manager.err_code, ERR_NONE);
    CHECK_EQ_INT(__sys_manager.output_stat, 0);
}

static void error_report_is_silent_without_a_pending_error(void)
{
    reset_env();
    __sys_manager.err_code = ERR_NONE;
    __sys_manager.output_stat = 1;

    error_event_report();

    CHECK_EQ_INT(fakes.ble_tx_cnt, 0);
    CHECK_EQ_INT(__sys_manager.output_stat, 1);
}

int main(void)
{
    RUN_TEST(output_dispenses_the_requested_slot);
    RUN_TEST(output_rejects_slot_zero_and_slots_past_the_last_load);
    RUN_TEST(output_accepts_the_last_valid_slot);
    RUN_TEST(output_requires_authorization_when_not_from_uart);
    RUN_TEST(output_is_rejected_while_a_dispense_is_running);
    RUN_TEST(charger_arms_the_close_timer_for_the_requested_hours);
    RUN_TEST(charger_clamps_the_request_to_eight_hours);
    RUN_TEST(charger_requires_authorization);
    RUN_TEST(charger_seconds_variant_uses_the_decimal_argument);
    RUN_TEST(charger_seconds_variant_requires_authorization);
    RUN_TEST(version_handler_notifies_the_firmware_version);
    RUN_TEST(voltage_handler_notifies_a_millivolt_reading);
    RUN_TEST(sleep_handler_drops_the_ble_link);
    RUN_TEST(factory_handler_needs_authorization);
    RUN_TEST(unimplemented_handlers_succeed_without_side_effects);
    RUN_TEST(cut_decimal_number_keeps_only_digits);
    RUN_TEST(cut_decimal_number_stops_at_the_requested_length);
    RUN_TEST(cut_decimal_number_returns_zero_without_digits);
    RUN_TEST(set_mac_address_rejects_names_that_are_not_16_digits);
    RUN_TEST(set_mac_address_reports_failure_when_flash_does_not_match);
    RUN_TEST(set_mac_address_is_refused_without_uart_or_authorization);
    RUN_TEST(rgb_handler_parses_channel_and_hex_colour);
    RUN_TEST(rgb_handler_treats_unparsable_colour_as_zero);
    RUN_TEST(st_handler_stores_the_requested_time);
    RUN_TEST(st_handler_stores_zero_without_a_number);
    RUN_TEST(traverse_dispatches_the_matching_handler);
    RUN_TEST(traverse_ignores_unknown_commands);
    RUN_TEST(traverse_propagates_handler_error_codes);
    RUN_TEST(error_report_notifies_and_clears_the_pending_error);
    RUN_TEST(error_report_is_silent_without_a_pending_error);

    return test_summary("AT");
}
