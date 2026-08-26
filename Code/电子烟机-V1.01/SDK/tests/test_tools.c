/* Unit tests for libs/Tools/tools.c */
#include <string.h>

#include "test_framework.h"
#include "tools.h"

static void byte_cpy_copies_requested_length_only(void)
{
    unsigned char src[6] = {1, 2, 3, 4, 5, 6};
    unsigned char dst[6] = {0};

    byte_cpy(dst, src, 4);

    CHECK_EQ_MEM(dst, "\x01\x02\x03\x04", 4);
    CHECK_EQ_INT(dst[4], 0);
    CHECK_EQ_INT(dst[5], 0);
}

static void byte_cpy_zero_length_is_noop(void)
{
    unsigned char src[2] = {0xAA, 0xBB};
    unsigned char dst[2] = {0x11, 0x22};

    byte_cpy(dst, src, 0);

    CHECK_EQ_INT(dst[0], 0x11);
    CHECK_EQ_INT(dst[1], 0x22);
}

static void compare_value_accepts_values_inside_window(void)
{
    CHECK_EQ_INT(compareValue(1000, 1000, 0), 1);
    CHECK_EQ_INT(compareValue(1010, 1000, 10), 1);
    CHECK_EQ_INT(compareValue(990, 1000, 10), 1);
    CHECK_EQ_INT(compareValue(1000, 1000, 10), 1);
}

static void compare_value_rejects_values_outside_window(void)
{
    CHECK_EQ_INT(compareValue(1011, 1000, 10), 0);
    CHECK_EQ_INT(compareValue(989, 1000, 10), 0);
    CHECK_EQ_INT(compareValue(0, 1000, 10), 0);
}

static void compare_value_handles_window_near_16bit_top(void)
{
    /* b + n would wrap past 0xFFFF, so the first branch clamps at 0xFFFF. */
    CHECK_EQ_INT(compareValue(0xFFFF, 0xFFF0, 0x20), 1);
    CHECK_EQ_INT(compareValue(0xFFD1, 0xFFF0, 0x20), 1);
    CHECK_EQ_INT(compareValue(0xFFC0, 0xFFF0, 0x20), 0);
}

static void uint4b_to_str_formats_four_digits(void)
{
    unsigned char str[5] = {0};

    CHECK(Uint4BToStr(1234, str) == str);
    CHECK_EQ_STR((char *)str, "1234");
}

static void uint4b_to_str_zero_pads_small_values(void)
{
    unsigned char str[5];

    Uint4BToStr(7, str);
    CHECK_EQ_STR((char *)str, "0007");

    Uint4BToStr(0, str);
    CHECK_EQ_STR((char *)str, "0000");

    Uint4BToStr(90, str);
    CHECK_EQ_STR((char *)str, "0090");
}

static void uint4b_to_str_keeps_low_four_digits_of_large_values(void)
{
    unsigned char str[5];

    Uint4BToStr(56789, str);
    CHECK_EQ_STR((char *)str, "6789");
}

static void hex_to_asc_decodes_digits_and_both_letter_cases(void)
{
    CHECK_EQ_INT(HexToAsc('0'), 0);
    CHECK_EQ_INT(HexToAsc('9'), 9);
    CHECK_EQ_INT(HexToAsc('A'), 10);
    CHECK_EQ_INT(HexToAsc('F'), 15);
    CHECK_EQ_INT(HexToAsc('a'), 10);
    CHECK_EQ_INT(HexToAsc('f'), 15);
}

static void hex_to_asc_rejects_non_hex_characters(void)
{
    /* 0xff is truncated to a signed char inside the function, hence -1. */
    CHECK_EQ_INT(HexToAsc('g'), -1);
    CHECK_EQ_INT(HexToAsc('/'), -1);
    CHECK_EQ_INT(HexToAsc(' '), -1);
}

static void string4byte_to_int_reads_three_digits_from_offset(void)
{
    unsigned char buffer[] = "x123";

    /* The thousands digit is deliberately skipped by the implementation. */
    CHECK_EQ_INT(String4ByteToInt(buffer, 0), 123);
}

static void string4byte_to_int_honours_offset(void)
{
    unsigned char buffer[] = "AT+O:0042";

    CHECK_EQ_INT(String4ByteToInt(buffer, 5), 42);
}

static void str_to_hex_decodes_uppercase_and_lowercase(void)
{
    unsigned char dst[4] = {0};
    unsigned char src[] = "0A1bFf";

    StrToHex(dst, src, 3);

    CHECK_EQ_INT(dst[0], 0x0A);
    CHECK_EQ_INT(dst[1], 0x1B);
    CHECK_EQ_INT(dst[2], 0xFF);
    CHECK_EQ_INT(dst[3], 0);
}

static void str_to_hex_zero_length_writes_nothing(void)
{
    unsigned char dst[2] = {0x5A, 0x5A};
    unsigned char src[] = "FFFF";

    StrToHex(dst, src, 0);

    CHECK_EQ_INT(dst[0], 0x5A);
    CHECK_EQ_INT(dst[1], 0x5A);
}

static void pack_string_appends_crlf_for_short_strings(void)
{
    char str[32] = "V:4200";
    char buffer[32] = {0};

    CHECK_EQ_INT(pack_string_to_buffer(str, buffer), 1);
    CHECK_EQ_STR(buffer, "V:4200\r\n");
}

static void pack_string_rejects_strings_of_20_chars_or_more(void)
{
    char str[32] = "01234567890123456789"; /* 20 chars */
    char buffer[32] = {0};

    CHECK_EQ_INT(pack_string_to_buffer(str, buffer), 0);
    CHECK_EQ_INT(buffer[0], 0);
}

static void pack_string_accepts_empty_string(void)
{
    char str[8] = "";
    char buffer[8] = {0};

    CHECK_EQ_INT(pack_string_to_buffer(str, buffer), 1);
    CHECK_EQ_STR(buffer, "\r\n");
}

int main(void)
{
    RUN_TEST(byte_cpy_copies_requested_length_only);
    RUN_TEST(byte_cpy_zero_length_is_noop);
    RUN_TEST(compare_value_accepts_values_inside_window);
    RUN_TEST(compare_value_rejects_values_outside_window);
    RUN_TEST(compare_value_handles_window_near_16bit_top);
    RUN_TEST(uint4b_to_str_formats_four_digits);
    RUN_TEST(uint4b_to_str_zero_pads_small_values);
    RUN_TEST(uint4b_to_str_keeps_low_four_digits_of_large_values);
    RUN_TEST(hex_to_asc_decodes_digits_and_both_letter_cases);
    RUN_TEST(hex_to_asc_rejects_non_hex_characters);
    RUN_TEST(string4byte_to_int_reads_three_digits_from_offset);
    RUN_TEST(string4byte_to_int_honours_offset);
    RUN_TEST(str_to_hex_decodes_uppercase_and_lowercase);
    RUN_TEST(str_to_hex_zero_length_writes_nothing);
    RUN_TEST(pack_string_appends_crlf_for_short_strings);
    RUN_TEST(pack_string_rejects_strings_of_20_chars_or_more);
    RUN_TEST(pack_string_accepts_empty_string);

    return test_summary("tools");
}
