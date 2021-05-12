#include "unmanaged_buffer.hxx"
#include "../lib/remem.hxx"

template<typename T>
size_t UnmanagedBuffer<T>::get_size() noexcept {
    return size;
}

template<typename T>
void UnmanagedBuffer<T>::set_size(size_t value) noexcept {
    size = value;
}

template<typename T>
void UnmanagedBuffer<T>::ignore_last() noexcept {
    --size;
}

template<typename T>
T* UnmanagedBuffer<T>::get_data() noexcept {
    return data;
}

template<typename T>
void UnmanagedBuffer<T>::write(const T* src, size_t length) {
    adjust_capacity(length);
    memcpy(data + size, src, sizeof(T) * length);
    size += length;
}

template<typename T>
void UnmanagedBuffer<T>::adjust_capacity(size_t length) {
    while(size + length > capacity) {
        if(data == NULL)
            data = (T*) REMEM_MALLOC(sizeof(T) * (capacity = UNMANAGED_BUFFER_START_SIZE), "Unmanaged buffer");
        else data = (T*) REMEM_REALLOC(data, sizeof(T) * (capacity *= 2));
    }
}