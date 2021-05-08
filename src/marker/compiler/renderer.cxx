#include "renderer.hxx"
#include "../def/html.dxx"
#include "../data/unmanaged_buffer.hxx"
#include "../def/macro.dxx"

namespace mker {
    Renderer::Renderer(AST&& ast) noexcept {
        this->ast = ast;
    }

    UnmanagedBuffer<char> Renderer::render() noexcept {
        UnmanagedBuffer<char> output;

        for(size_t i = 0; i < ast.size(); ++i) {
            render_node(ast[i], output);

            if(i < ast.size() - 1)
                output.write("\n", 1);
        }

        output.write("\0", 1);
        output.ignore_last();
        
        return output;
    }

    void Renderer::render_node(const ASTNode& node, UnmanagedBuffer<char>& output) noexcept {
        switch(node.type) {
            case ASTNodeType::PARAGRAPH: {
                output.write(HTML_TAG_OPEN_PARAGRAPH, sizeof(HTML_TAG_OPEN_PARAGRAPH) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_PARAGRAPH, sizeof(HTML_TAG_CLOSE_PARAGRAPH) - 1);

                break;
            }
            case ASTNodeType::HEADING1: {
                output.write(HTML_TAG_OPEN_HEADING(1), sizeof(HTML_TAG_OPEN_HEADING(1)) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_HEADING(1), sizeof(HTML_TAG_CLOSE_HEADING(1)) - 1);

                break;
            }
            case ASTNodeType::HEADING2: {
                output.write(HTML_TAG_OPEN_HEADING(2), sizeof(HTML_TAG_OPEN_HEADING(2)) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_HEADING(2), sizeof(HTML_TAG_CLOSE_HEADING(2)) - 1);

                break;
            }
            case ASTNodeType::HEADING3: {
                output.write(HTML_TAG_OPEN_HEADING(3), sizeof(HTML_TAG_OPEN_HEADING(3)) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_HEADING(3), sizeof(HTML_TAG_CLOSE_HEADING(3)) - 1);

                break;
            }
            case ASTNodeType::HEADING4: {
                output.write(HTML_TAG_OPEN_HEADING(4), sizeof(HTML_TAG_OPEN_HEADING(4)) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_HEADING(4), sizeof(HTML_TAG_CLOSE_HEADING(4)) - 1);

                break;
            }
            case ASTNodeType::HEADING5: {
                output.write(HTML_TAG_OPEN_HEADING(5), sizeof(HTML_TAG_OPEN_HEADING(5)) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_HEADING(5), sizeof(HTML_TAG_CLOSE_HEADING(5)) - 1);

                break;
            }
            case ASTNodeType::HEADING6: {
                output.write(HTML_TAG_OPEN_HEADING(6), sizeof(HTML_TAG_OPEN_HEADING(6)) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_HEADING(6), sizeof(HTML_TAG_CLOSE_HEADING(6)) - 1);

                break;
            }
            case ASTNodeType::LIST: {
                output.write(HTML_TAG_OPEN_LIST_UNORDERED, sizeof(HTML_TAG_OPEN_LIST_UNORDERED) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_LIST_UNORDERED, sizeof(HTML_TAG_CLOSE_LIST_UNORDERED) - 1);

                break;
            }
            case ASTNodeType::LIST_ELEMENT: {
                output.write(HTML_TAG_OPEN_LIST_ELEMENT, sizeof(HTML_TAG_OPEN_LIST_ELEMENT) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_LIST_ELEMENT, sizeof(HTML_TAG_CLOSE_LIST_ELEMENT) - 1);

                break;
            }
            case ASTNodeType::TEXT: {
                output.write(node.start, node.end - node.start);

                break;
            }
            case ASTNodeType::EMPHASIS: {
                output.write(HTML_TAG_OPEN_EMPHASIS, sizeof(HTML_TAG_OPEN_EMPHASIS) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_EMPHASIS, sizeof(HTML_TAG_CLOSE_EMPHASIS) - 1);

                break;
            }
            case ASTNodeType::STRONG: {
                output.write(HTML_TAG_OPEN_STRONG, sizeof(HTML_TAG_OPEN_STRONG) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_STRONG, sizeof(HTML_TAG_CLOSE_STRONG) - 1);

                break;
            }
            case ASTNodeType::UNDERLINE: {
                output.write(HTML_TAG_OPEN_UNDERLINE, sizeof(HTML_TAG_OPEN_UNDERLINE) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_UNDERLINE, sizeof(HTML_TAG_CLOSE_UNDERLINE) - 1);

                break;
            }
            case ASTNodeType::STRIKETHROUGH: {
                output.write(HTML_TAG_OPEN_STRIKETHROUGH, sizeof(HTML_TAG_OPEN_STRIKETHROUGH) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_STRIKETHROUGH, sizeof(HTML_TAG_CLOSE_STRIKETHROUGH) - 1);

                break;
            }
            case ASTNodeType::TAG: {
                output.write(HTML_TAG_OPEN, sizeof(HTML_TAG_OPEN) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE, sizeof(HTML_TAG_CLOSE) - 1);

                break;
            }
            case ASTNodeType::SOFTBREAK: {
                output.write("\n", 1);

                break;
            }
        }
    }
    void Renderer::render_children(const ASTNode& node, UnmanagedBuffer<char>& output) noexcept  {
        for(size_t i = 0; i < node.children.size(); ++i)
            render_node(node.children[i], output);
    }
}