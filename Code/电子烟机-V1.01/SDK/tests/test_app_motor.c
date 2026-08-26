/* Unit tests for libs/drv/app_motor.c */
#include "app_motor.h"
#include "fakes.h"
#include "test_framework.h"

void motor_rest(void);

#define MOTOR_DATA_IO GPIO_P16

static void send_msg_emits_one_byte_per_position(void)
{
    const uint8_t expected[7] = {0xF0, 0xE1, 0xD2, 0xC3, 0xB4, 0xA5, 0x96};
    uint8_t position;

    fakes_reset();
    for (position = 0; position < 7; position++) {
        motor_send_msg(position);
        CHECK_EQ_INT(fakes.uart2_tx_cnt, position + 1);
        CHECK_EQ_INT(fakes.uart2_tx[position], expected[position]);
    }
}

static void motor_rest_sends_the_home_position(void)
{
    fakes_reset();

    motor_rest();

    CHECK_EQ_INT(fakes.uart2_tx_cnt, 1);
    CHECK_EQ_INT(fakes.uart2_tx[0], 0xF0);
}

static void receive_samples_eight_bits_when_start_bit_arrives(void)
{
    uint8_t data;

    fakes_reset();
    fakes.input_level = 0; /* line already low: start bit seen immediately */

    data = motor_receive();

    /* Every sampled bit reads 0, so the decoded byte is 0. */
    CHECK_EQ_INT(data, 0x00);
    CHECK(fake_gpio_configured(MOTOR_DATA_IO));
    /* The bus is handed back to UART2 at 9600 baud once framing is done. */
    CHECK_EQ_INT(fakes.uart2_init_cnt, 1);
    CHECK_EQ_INT(fakes.uart2_last_baudrate, 9600);
}

static void receive_reports_0xff_when_the_line_never_goes_low(void)
{
    uint8_t data;

    fakes_reset();
    fakes.input_level = 1; /* idle high forever -> timeout path */

    data = motor_receive();

    CHECK_EQ_INT(data, 0xFF);
    CHECK_EQ_INT(fakes.uart2_init_cnt, 1);
}

int main(void)
{
    RUN_TEST(send_msg_emits_one_byte_per_position);
    RUN_TEST(motor_rest_sends_the_home_position);
    RUN_TEST(receive_samples_eight_bits_when_start_bit_arrives);
    RUN_TEST(receive_reports_0xff_when_the_line_never_goes_low);

    return test_summary("app_motor");
}
