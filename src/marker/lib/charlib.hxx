#ifndef MARKER_LIB_CHARLIB_HXX_GUARD
#define MARKER_LIB_CHARLIB_HXX_GUARD

#include <cstdint>
#include <cstddef>

namespace charlib {
    bool is_lower_alpha (char c);
    bool is_upper_alpha (char c);
    bool is_alpha       (char c);
    bool is_digit       (char c);

    bool is_alphanum(char c);

    size_t num_to_buffer(char* buffer, uint32_t num);

    uint32_t roman_char(char c);
    bool     roman_parse(const char* src,  size_t size, uint32_t& result);

    bool alpha_parse(const char* src, size_t size, uint32_t& result);
    bool num_parse(const char* src, size_t size, uint32_t& result);
}

#endif
