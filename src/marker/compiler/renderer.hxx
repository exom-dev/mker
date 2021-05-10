#ifndef MARKER_RENDERER_HXX_GUARD
#define MARKER_RENDERER_HXX_GUARD

#include "../data/ast.hxx"
#include "../data/unmanaged_buffer.hxx"

namespace mker {
    class Renderer {
        private:
            AST ast;

        public:
            explicit Renderer(AST&& ast) noexcept;

            UnmanagedBuffer<char> render() noexcept;

        private:
            void render_node     (const ASTNode& node, UnmanagedBuffer<char>& output) noexcept;
            void render_children (const ASTNode& node, UnmanagedBuffer<char>& output) noexcept;
            void render_char     (const char* start, size_t& index, size_t size, UnmanagedBuffer<char>& output) noexcept;
    };
}

#endif
