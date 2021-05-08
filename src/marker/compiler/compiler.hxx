#ifndef MARKER_COMPILER_HXX_GUARD
#define MARKER_COMPILER_HXX_GUARD

#include "../data/unmanaged_buffer.hxx"

namespace mker {
    class Compiler {
        private:
            const char *src;

        public:
            explicit Compiler(const char *source) noexcept;

            UnmanagedBuffer<char> compile() noexcept;
    };
}

#endif
