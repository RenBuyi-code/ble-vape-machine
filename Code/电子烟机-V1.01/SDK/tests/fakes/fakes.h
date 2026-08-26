/*
 * Test doubles for the BK3432 hardware/SDK dependencies of the modules under
 * test. Every fake records its calls so tests can assert on the side effects
 * the firmware performs.
 */
#ifndef _FAKES_H_
#define _FAKES_H_

#include <stdint.h>

#define FAKE_LOG_LEN 64

typedef struct {
    uint8_t gpio;
    uint8_t val;
} fake_gpio_write;

typedef struct {
    uint8_t gpio;
    int dir;
    int pull;
} fake_gpio_cfg;

typedef struct {
    uint32_t address;
    uint8_t data[32];
    uint32_t length;
} fake_flash_write;

typedef struct {
    uint16_t msg_id;
    uint16_t task_id;
    uint32_t delay;
} fake_timer_set;

typedef struct {
    /* gpio */
    fake_gpio_write writes[FAKE_LOG_LEN];
    uint32_t write_cnt;
    fake_gpio_cfg configs[FAKE_LOG_LEN];
    uint32_t config_cnt;
    uint8_t input_level; /* value returned by gpio_get_input() */

    /* uart2 (motor bus) */
    uint8_t uart2_tx[FAKE_LOG_LEN];
    uint32_t uart2_tx_cnt;
    uint32_t uart2_init_cnt;
    uint32_t uart2_last_baudrate;

    /* ble notify */
    char ble_tx[FAKE_LOG_LEN][32];
    uint32_t ble_tx_cnt;

    /* flash */
    fake_flash_write flash_writes[FAKE_LOG_LEN];
    uint32_t flash_write_cnt;

    /* kernel timers */
    fake_timer_set timers[FAKE_LOG_LEN];
    uint32_t timer_cnt;

    /* misc peripherals */
    uint16_t adc_value; /* value returned by adc_get_value() */
    uint32_t wdt_feed_cnt;
    uint32_t wdt_enable_cnt;
    uint32_t disconnect_cnt;
    uint32_t delay_ms_total;

    /* rgb */
    uint32_t rgb_set_cnt;
    uint8_t rgb_last_ch;
    unsigned long rgb_last_color;
} fake_state_t;

extern fake_state_t fakes;

void fakes_reset(void);
/* Number of times gpio_set(gpio, val) was called. */
uint32_t fake_gpio_write_count(uint8_t gpio, uint8_t val);
/* Level of the last gpio_set() on a pin, or -1 when never written. */
int fake_gpio_level(uint8_t gpio);
int fake_gpio_configured(uint8_t gpio);

#endif
