#include "charlib.hxx"

namespace charlib {
    bool is_alphanum(char c) {
        return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }
}
