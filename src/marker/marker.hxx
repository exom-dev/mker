#ifndef MARKER_HXX_GUARD
#define MARKER_HXX_GUARD

#include "data/unmanaged_buffer.hxx"

#include <exception>

namespace mker {
    class NoSourceLoaded : public std::exception {
        const char* what() const noexcept override {
            return "No source was loaded; try calling load(const char*) before compile()";
        }
    };

    class Marker {
        private:
            const char* src;
        public:
                     Marker ()                   noexcept;
            explicit Marker (const char* source) noexcept;

            void  load    (const char* source) noexcept;
            UnmanagedBuffer<char> compile ();
    };
}

#endif
