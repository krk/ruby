#include "yarp/enc/yp_encoding.h"

typedef uint16_t gbk_codepoint_t;

static gbk_codepoint_t
gbk_codepoint(const char *c, size_t *width) {
    const unsigned char *uc = (const unsigned char *) c;

    // These are the single byte characters.
    if (*uc < 0x80) {
        *width = 1;
        return *uc;
    }

    // These are the double byte characters.
    if (
        ((uc[0] >= 0xA1 && uc[0] <= 0xA9) && (uc[1] >= 0xA1 && uc[1] <= 0xFE)) || // GBK/1
        ((uc[0] >= 0xB0 && uc[0] <= 0xF7) && (uc[1] >= 0xA1 && uc[1] <= 0xFE)) || // GBK/2
        ((uc[0] >= 0x81 && uc[0] <= 0xA0) && (uc[1] >= 0x40 && uc[1] <= 0xFE) && (uc[1] != 0x7F)) || // GBK/3
        ((uc[0] >= 0xAA && uc[0] <= 0xFE) && (uc[1] >= 0x40 && uc[1] <= 0xA0) && (uc[1] != 0x7F)) || // GBK/4
        ((uc[0] >= 0xA8 && uc[0] <= 0xA9) && (uc[1] >= 0x40 && uc[1] <= 0xA0) && (uc[1] != 0x7F)) // GBK/5
    ) {
        *width = 2;
        return (gbk_codepoint_t) (uc[0] << 8 | uc[1]);
    }

    *width = 0;
    return 0;
}

static size_t
yp_encoding_gbk_char_width(const char *c) {
    size_t width;
    gbk_codepoint(c, &width);

    return width;
}

static size_t
yp_encoding_gbk_alpha_char(const char *c) {
    size_t width;
    gbk_codepoint_t codepoint = gbk_codepoint(c, &width);

    if (width == 1) {
        const char value = (const char) codepoint;
        return yp_encoding_ascii_alpha_char(&value);
    } else {
        return 0;
    }
}

static size_t
yp_encoding_gbk_alnum_char(const char *c) {
    size_t width;
    gbk_codepoint_t codepoint = gbk_codepoint(c, &width);

    if (width == 1) {
        const char value = (const char) codepoint;
        return yp_encoding_ascii_alnum_char(&value);
    } else {
        return 0;
    }
}

static bool
yp_encoding_gbk_isupper_char(const char *c) {
    size_t width;
    gbk_codepoint_t codepoint = gbk_codepoint(c, &width);

    if (width == 1) {
        const char value = (const char) codepoint;
        return yp_encoding_ascii_isupper_char(&value);
    } else {
        return false;
    }
}

yp_encoding_t yp_encoding_gbk = {
    .name = "gbk",
    .char_width = yp_encoding_gbk_char_width,
    .alnum_char = yp_encoding_gbk_alnum_char,
    .alpha_char = yp_encoding_gbk_alpha_char,
    .isupper_char = yp_encoding_gbk_isupper_char
};
