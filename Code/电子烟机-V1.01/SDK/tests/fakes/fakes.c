#include "fakes.h"

#include <stdarg.h>
#include <string.h>

#include "adc.h"
#include "app.h"
#include "app_fff0.h"
#include "drv_rgb.h"
#include "flash.h"
#include "gpio.h"
#include "ke_timer.h"
#include "uart.h"
#include "uart2.h"
#include "wdt.h"

fake_state_t fakes;
uint32_t fake_reg_scratch;
volatile bool uart_rx_done;

void fakes_reset(void)
{
    memset(&fakes, 0, sizeof(fakes));
    fake_reg_scratch = 0;
    uart_rx_done = false;
}

uint32_t fake_gpio_write_count(uint8_t gpio, uint8_t val)
{
    uint32_t i;
    uint32_t cnt = 0;

    for (i = 0; i < fakes.write_cnt; i++) {
        if (fakes.writes[i].gpio == gpio && fakes.writes[i].val == val) {
            cnt++;
        }
    }
    return cnt;
}

int fake_gpio_level(uint8_t gpio)
{
    uint32_t i = fakes.write_cnt;

    while (i-- > 0) {
        if (fakes.writes[i].gpio == gpio) {
            return fakes.writes[i].val;
        }
    }
    return -1;
}

int fake_gpio_configured(uint8_t gpio)
{
    uint32_t i;

    for (i = 0; i < fakes.config_cnt; i++) {
        if (fakes.configs[i].gpio == gpio) {
            return 1;
        }
    }
    return 0;
}

/* ---------------- gpio ---------------- */

void gpio_init(void) {}

void gpio_config(uint8_t gpio, Dir_Type dir, Pull_Type pull)
{
    if (fakes.config_cnt < FAKE_LOG_LEN) {
        fakes.configs[fakes.config_cnt].gpio = gpio;
        fakes.configs[fakes.config_cnt].dir = (int)dir;
        fakes.configs[fakes.config_cnt].pull = (int)pull;
        fakes.config_cnt++;
    }
}

void gpio_set(uint8_t gpio, uint8_t val)
{
    if (fakes.write_cnt < FAKE_LOG_LEN) {
        fakes.writes[fakes.write_cnt].gpio = gpio;
        fakes.writes[fakes.write_cnt].val = val;
        fakes.write_cnt++;
    }
}

uint8_t gpio_get_input(uint8_t gpio)
{
    (void)gpio;
    return fakes.input_level;
}

uint8_t gpio_get_output(uint8_t gpio)
{
    int level = fake_gpio_level(gpio);

    return level < 0 ? 0 : (uint8_t)level;
}

/* ---------------- uart ---------------- */

int uart_printf(const char *fmt, ...)
{
    (void)fmt;
    return 0;
}

int uart_printf_null(const char *fmt, ...)
{
    (void)fmt;
    return 0;
}

void uart_init(uint32_t baudrate) { (void)baudrate; }

void uart_send(unsigned char *buff, int len)
{
    (void)buff;
    (void)len;
}

void uart2_init(uint32_t baudrate)
{
    fakes.uart2_init_cnt++;
    fakes.uart2_last_baudrate = baudrate;
}

void uart2_send(unsigned char *buff, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (fakes.uart2_tx_cnt < FAKE_LOG_LEN) {
            fakes.uart2_tx[fakes.uart2_tx_cnt++] = buff[i];
        }
    }
}

/* ---------------- ble ---------------- */

void app_fff1_send_lvl(uint8_t *buf, uint8_t len)
{
    uint8_t copy_len = len;

    if (fakes.ble_tx_cnt >= FAKE_LOG_LEN) {
        return;
    }
    if (copy_len > sizeof(fakes.ble_tx[0]) - 1) {
        copy_len = sizeof(fakes.ble_tx[0]) - 1;
    }
    memcpy(fakes.ble_tx[fakes.ble_tx_cnt], buf, copy_len);
    fakes.ble_tx[fakes.ble_tx_cnt][copy_len] = '\0';
    fakes.ble_tx_cnt++;
}

void appm_disconnect(void) { fakes.disconnect_cnt++; }

void appm_update_adv_data(uint8_t *data, uint8_t len)
{
    (void)data;
    (void)len;
}

/* ---------------- flash ---------------- */

void Write_NVR_Flash(uint32_t address, uint8_t *buffer, uint32_t length)
{
    uint32_t copy_len = length;

    if (fakes.flash_write_cnt >= FAKE_LOG_LEN) {
        return;
    }
    if (copy_len > sizeof(fakes.flash_writes[0].data)) {
        copy_len = sizeof(fakes.flash_writes[0].data);
    }
    fakes.flash_writes[fakes.flash_write_cnt].address = address;
    fakes.flash_writes[fakes.flash_write_cnt].length = length;
    memcpy(fakes.flash_writes[fakes.flash_write_cnt].data, buffer, copy_len);
    fakes.flash_write_cnt++;
}

void Read_NVR_Flash(uint32_t address, uint8_t *buffer, uint32_t length)
{
    uint32_t i = fakes.flash_write_cnt;

    /* Replay the most recent write to the same address, like real NVR does. */
    while (i-- > 0) {
        if (fakes.flash_writes[i].address == address) {
            uint32_t copy_len = length;

            if (copy_len > sizeof(fakes.flash_writes[0].data)) {
                copy_len = sizeof(fakes.flash_writes[0].data);
            }
            memcpy(buffer, fakes.flash_writes[i].data, copy_len);
            return;
        }
    }
}

/* ---------------- kernel / misc ---------------- */

void ke_timer_set(uint16_t timer_id, uint16_t task_id, uint32_t delay)
{
    if (fakes.timer_cnt < FAKE_LOG_LEN) {
        fakes.timers[fakes.timer_cnt].msg_id = timer_id;
        fakes.timers[fakes.timer_cnt].task_id = task_id;
        fakes.timers[fakes.timer_cnt].delay = delay;
        fakes.timer_cnt++;
    }
}

void ke_timer_clear(uint16_t timer_id, uint16_t task_id)
{
    (void)timer_id;
    (void)task_id;
}

uint16_t adc_get_value(uint8_t channel)
{
    (void)channel;
    return fakes.adc_value;
}

void wdt_feed(uint16_t wdt_cnt)
{
    (void)wdt_cnt;
    fakes.wdt_feed_cnt++;
}

void wdt_enable(uint16_t wdt_cnt)
{
    (void)wdt_cnt;
    fakes.wdt_enable_cnt++;
}

void wdt_disable(void) {}

void switch_clk(uint8_t clk) { (void)clk; }

void Delay_ms(int num) { fakes.delay_ms_total += (uint32_t)num; }

void Delay_us(int num) { (void)num; }

void drv_ws2812_gpio_init(void) {}

void drv_ws2812_set_color(uint8_t ch, unsigned long RGB)
{
    fakes.rgb_set_cnt++;
    fakes.rgb_last_ch = ch;
    fakes.rgb_last_color = RGB;
}
