#include "str.hxx"
#include "remem.hxx"

#include <cstring>

char* str_dup(const char* str) {
    size_t length = strlen(str) + 1;
    char* ptr = reinterpret_cast<char*>(REMEM_MALLOC(length, "StrDup String"));

    memcpy(ptr, str, length);
    return ptr;
}

char* str_dup(const char* str, size_t size) {
    char* ptr = reinterpret_cast<char*>(REMEM_MALLOC(size + 1, "StrDup String"));

    memcpy(ptr, str, size);
    ptr[size] = '\0';

    return ptr;
}