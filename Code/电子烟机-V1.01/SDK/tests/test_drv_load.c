/* Unit tests for libs/drv/drv_load.c, drv_charger.c and drv_battery.c */
#include "drv_battery.h"
#include "drv_charger.h"
#include "drv_load.h"
#include "fakes.h"
#include "test_framework.h"

static void load_open_drives_the_mapped_pin_high(void)
{
    fakes_reset();

    CHECK_EQ_INT(hal_load_open(1), 1);
    CHECK_EQ_INT(fakes.write_cnt, 1);
    CHECK_EQ_INT(fakes.writes[0].gpio, L1_GPIO);
    CHECK_EQ_INT(fakes.writes[0].val, 1);
}

static void load_open_maps_every_valid_channel(void)
{
    const uint8_t expected[MAX_LOAD_NUMBER - 1] = {L1_GPIO, L2_GPIO, L3_GPIO,
                                                   L4_GPIO, L5_GPIO, L6_GPIO};
    uint8_t load;

    fakes_reset();
    for (load = 1; load < MAX_LOAD_NUMBER; load++) {
        CHECK_EQ_INT(hal_load_open(load), 1);
        CHECK_EQ_INT(fakes.writes[load - 1].gpio, expected[load - 1]);
        CHECK_EQ_INT(fakes.writes[load - 1].val, 1);
    }
    CHECK_EQ_INT(fakes.write_cnt, MAX_LOAD_NUMBER - 1);
}

static void load_open_rejects_out_of_range_channels(void)
{
    fakes_reset();

    /* Channel 0 is the 0xFF placeholder in the pin table, and MAX_LOAD_NUMBER
     * is one past the last usable load. Neither may touch a pin. */
    CHECK_EQ_INT(hal_load_open(0), 0);
    CHECK_EQ_INT(hal_load_open(MAX_LOAD_NUMBER), 0);
    CHECK_EQ_INT(hal_load_open(255), 0);
    CHECK_EQ_INT(fakes.write_cnt, 0);
}

static void load_close_drives_the_mapped_pin_low(void)
{
    fakes_reset();

    CHECK_EQ_INT(hal_load_close(3), 1);
    CHECK_EQ_INT(fakes.write_cnt, 1);
    CHECK_EQ_INT(fakes.writes[0].gpio, L3_GPIO);
    CHECK_EQ_INT(fakes.writes[0].val, 0);
}

static void load_close_rejects_out_of_range_channels(void)
{
    fakes_reset();

    CHECK_EQ_INT(hal_load_close(0), 0);
    CHECK_EQ_INT(hal_load_close(MAX_LOAD_NUMBER), 0);
    CHECK_EQ_INT(fakes.write_cnt, 0);
}

static void gpio_init_configures_all_loads_low(void)
{
    const uint8_t load_pins[MAX_LOAD_NUMBER - 1] = {L1_GPIO, L2_GPIO, L3_GPIO,
                                                    L4_GPIO, L5_GPIO, L6_GPIO};
    uint8_t i;

    fakes_reset();
    hal_load_gpio_init();

    CHECK(fake_gpio_configured(L_POWER_GPIO));
    CHECK(fake_gpio_configured(LED_GPIO));
    CHECK(fake_gpio_configured(VBAT_AIN_CTL));
    CHECK(fake_gpio_configured(CHARGER_PW_GPIO));
    CHECK_EQ_INT(fake_gpio_level(L_POWER_GPIO), 0);
    CHECK_EQ_INT(fake_gpio_level(CHARGER_PW_GPIO), 0);
    for (i = 0; i < MAX_LOAD_NUMBER - 1; i++) {
        CHECK(fake_gpio_configured(load_pins[i]));
        CHECK_EQ_INT(fake_gpio_level(load_pins[i]), 0);
    }
}

static void led_helpers_toggle_the_led_pin(void)
{
    fakes_reset();

    hal_led_open();
    CHECK_EQ_INT(fake_gpio_level(LED_GPIO), 1);

    hal_led_close();
    CHECK_EQ_INT(fake_gpio_level(LED_GPIO), 0);
}

static void load_power_open_also_enables_the_battery_divider(void)
{
    fakes_reset();

    hal_load_power_open();
    CHECK_EQ_INT(fake_gpio_level(L_POWER_GPIO), 1);
    CHECK_EQ_INT(fake_gpio_level(VBAT_AIN_CTL), 1);

    hal_load_power_close();
    CHECK_EQ_INT(fake_gpio_level(L_POWER_GPIO), 0);
    CHECK_EQ_INT(fake_gpio_level(VBAT_AIN_CTL), 0);
}

static void get_delay_time_returns_100ms_for_healthy_packs(void)
{
    CHECK_EQ_INT(get_delay_time(6999), 100);
    CHECK_EQ_INT(get_delay_time(6000), 100);
    CHECK_EQ_INT(get_delay_time(5999), 100);
    CHECK_EQ_INT(get_delay_time(5000), 100);
}

static void get_delay_time_stretches_as_voltage_drops(void)
{
    CHECK_EQ_INT(get_delay_time(4999), 150);
    CHECK_EQ_INT(get_delay_time(4000), 150);
    CHECK_EQ_INT(get_delay_time(3999), 140);
    CHECK_EQ_INT(get_delay_time(2800), 140);
    CHECK_EQ_INT(get_delay_time(2799), 180);
    CHECK_EQ_INT(get_delay_time(0), 180);
}

static void get_delay_time_defaults_to_100ms_above_the_table(void)
{
    /* 7000 mV and up falls through every branch and keeps the initial value. */
    CHECK_EQ_INT(get_delay_time(7000), 100);
    CHECK_EQ_INT(get_delay_time(65535), 100);
}

static void charger_helpers_toggle_the_charge_enable_pin(void)
{
    fakes_reset();

    hal_charger_pw_gpio_init();
    CHECK(fake_gpio_configured(CHARGER_PW_GPIO));
    CHECK_EQ_INT(fake_gpio_level(CHARGER_PW_GPIO), 0);

    hal_charger_pw_open();
    CHECK_EQ_INT(fake_gpio_level(CHARGER_PW_GPIO), 1);

    hal_charger_pw_close();
    CHECK_EQ_INT(fake_gpio_level(CHARGER_PW_GPIO), 0);
}

static void battery_voltage_is_monotonic_in_the_adc_reading(void)
{
    int low;
    int high;

    fakes_reset();
    fakes.adc_value = 400;
    low = drv_get_battery_vol();

    fakes.adc_value = 800;
    high = drv_get_battery_vol();

    CHECK(high > low);
}

static void battery_voltage_matches_the_divider_fit(void)
{
    fakes_reset();

    /* 620 counts is a nominal ~4.1 V cell through the 110k/20k divider. */
    fakes.adc_value = 620;
    CHECK_EQ_INT(drv_get_battery_vol(), 4031);

    fakes.adc_value = 0;
    CHECK_EQ_INT(drv_get_battery_vol(), -455);
}

int main(void)
{
    RUN_TEST(load_open_drives_the_mapped_pin_high);
    RUN_TEST(load_open_maps_every_valid_channel);
    RUN_TEST(load_open_rejects_out_of_range_channels);
    RUN_TEST(load_close_drives_the_mapped_pin_low);
    RUN_TEST(load_close_rejects_out_of_range_channels);
    RUN_TEST(gpio_init_configures_all_loads_low);
    RUN_TEST(led_helpers_toggle_the_led_pin);
    RUN_TEST(load_power_open_also_enables_the_battery_divider);
    RUN_TEST(get_delay_time_returns_100ms_for_healthy_packs);
    RUN_TEST(get_delay_time_stretches_as_voltage_drops);
    RUN_TEST(get_delay_time_defaults_to_100ms_above_the_table);
    RUN_TEST(charger_helpers_toggle_the_charge_enable_pin);
    RUN_TEST(battery_voltage_is_monotonic_in_the_adc_reading);
    RUN_TEST(battery_voltage_matches_the_divider_fit);

    return test_summary("drv_load");
}
