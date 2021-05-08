#include "marker.hxx"

#include "compiler/compiler.hxx"

namespace mker {
    Marker::Marker() noexcept {
        src = nullptr;
    }

    Marker::Marker(const char* source) noexcept {
        load(source);
    }

    void Marker::load(const char* source) noexcept {
        src = source;
    }

    UnmanagedBuffer<char> Marker::compile() {
        if(src == nullptr)
            throw NoSourceLoaded();

        return Compiler(src).compile();
    }
}