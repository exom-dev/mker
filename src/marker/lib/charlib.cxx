#include "charlib.hxx"

namespace charlib {
    bool is_lower_alpha(char c) {
        return (c >= 'a' && c <= 'z');
    }

    bool is_upper_alpha(char c) {
        return (c >= 'A' && c <= 'Z');
    }

    bool is_alpha(char c) {
        return is_lower_alpha(c) || is_upper_alpha(c);
    }

    bool is_digit(char c) {
        return (c >= '0' && c <= '9');
    }

    bool is_alphanum(char c) {
        return is_alpha(c) || is_digit(c);
    }

    size_t num_to_buffer(char* buffer, uint32_t num) {
        uint32_t backupNum = num;
        size_t length = 0;

        // Count the number of digits.
        do {
            ++length;
            backupNum /= 10;
        } while(backupNum != 0);

        char* ptr = buffer + length - 1;

        // Write the digits.
        for(size_t i = length; i > 0; --i, --ptr) {
            *ptr = (char) (num % 10 + '0');

            num /= 10;
        }

        return length;
    }

    uint32_t roman_char(char c) {
        switch(c) {
            case 'i':
            case 'I':
                return 1;
            case 'v':
            case 'V':
                return 5;
            case 'x':
            case 'X':
                return 10;
            case 'l':
            case 'L':
                return 50;
            case 'c':
            case 'C':
                return 100;
            case 'd':
            case 'D':
                return 500;
            case 'm':
            case 'M':
                return 1000;
            default:
                return 0;
        }
    }

    bool roman_parse(const char* src,  size_t size, uint32_t& result) {
        result = 0;

        // Check the first character, then check the i+1th character.
        if(roman_char(src[0]) == 0)
            return false;

        for(size_t i = 0; i < size - 1; ++i) {
            if(roman_char(src[i + 1]) == 0)
                return false;

            uint32_t val = roman_char(src[i]);

            if(val < roman_char(src[i + 1]))
                result -= val;
            else result += val;
        }

        // Add the last character.
        result += roman_char(src[size - 1]);

        return true;
    }

    bool alpha_parse(const char* src, size_t size, uint32_t& result) {
        // (26^6) - 1 can be stored in a 32-bit int. (26^7) - 1 cannot.
        // Limit to 6 characters.
        if(size < 1 || size > 6)
            return false;

        const char* ptr = src + size - 1;

        uint32_t factor = 1;

        result = 0;

        while(true) {
            if(is_lower_alpha(*ptr)) {
                result += factor * ((*ptr) - 'a' + 1);
            } else if(is_upper_alpha(*ptr)) {
                result += factor * ((*ptr) - 'A' + 1);
            } else {
                return false;
            }

            // If 'src' is somehow 0, decrementing the pointer would overflow -> infinite loop.
            // Manually break, and remove the while loop condition.
            if(ptr == src)
                break;

            --ptr;

            factor *= ('z' - 'a' + 1); // 26
        }

        return true;
    }

    bool num_parse(const char* src, size_t size, uint32_t& result) {
        // Limit to 9 characters.
        if(size < 1 || size > 9)
            return false;

        const char* ptr = src + size - 1;

        uint32_t factor = 1;

        result = 0;

        while(true) {
            if(!is_digit(*ptr))
                return false;

            result += factor * ((*ptr) - '0');

            // If 'src' is somehow 0, decrementing the pointer would overflow -> infinite loop.
            // Manually break, and remove the while loop condition.
            if(ptr == src)
                break;

            --ptr;

            factor *= 10;
        }

        return true;
    }
}
