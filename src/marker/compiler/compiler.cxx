#include "parser.hxx"
#include "renderer.hxx"
#include "compiler.hxx" // Include re::mem only after including std::vector.

namespace mker {
    Compiler::Compiler(const char* source) noexcept {
        src = source;
    }

    UnmanagedBuffer<char> Compiler::compile() noexcept {
        return Renderer(std::move(Parser(src).parse())).render();
    }
}