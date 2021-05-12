#ifndef MARKER_DATA_UNMANAGED_BUFFER_HXX_GUARD
#define MARKER_DATA_UNMANAGED_BUFFER_HXX_GUARD

#include <cstddef>
#include <cstring>

#define UNMANAGED_BUFFER_START_SIZE 8

/// Creates and holds a buffer that can expand. Unmanaged because it doesn't free the
/// memory when going out of scope. Useful when the buffer data may be used in a parent
/// function or anywhere else (like the Node environment), and must not be freed right
/// away. In case you forget to properly free the buffer when it's no longer needed,
/// use re::mem to trace and fix the memory leak.
template<typename T>
class UnmanagedBuffer {
    T* data;
    size_t size;
    size_t capacity;

public:
    UnmanagedBuffer() : data(NULL), size(0), capacity(0) { }

    T*     get_data() noexcept;
    size_t get_size() noexcept;
    void   set_size(size_t value) noexcept;
    void   ignore_last() noexcept;
    void   write(const T* src, size_t length);

    void adjust_capacity(size_t length);
};

#include "unmanaged_buffer.txx"

#endif
