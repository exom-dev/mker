#include "renderer.hxx"
#include "../def/html.dxx"
#include "../data/unmanaged_buffer.hxx"
#include "../def/macro.dxx"
#include "../def/limits.dxx"
#include "../lib/charlib.hxx"

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

        // Write the null character, but don't count it for the final size.
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
            case ASTNodeType::UNORDERED_LIST: {
                output.write(HTML_TAG_OPEN_LIST_UNORDERED, sizeof(HTML_TAG_OPEN_LIST_UNORDERED) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_LIST_UNORDERED, sizeof(HTML_TAG_CLOSE_LIST_UNORDERED) - 1);

                break;
            }
            case ASTNodeType::UNORDERED_LIST_ELEMENT: {
                output.write(HTML_TAG_OPEN_LIST_ELEMENT, sizeof(HTML_TAG_OPEN_LIST_ELEMENT) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_LIST_ELEMENT, sizeof(HTML_TAG_CLOSE_LIST_ELEMENT) - 1);

                break;
            }
            case ASTNodeType::ORDERED_LIST: {
                // Exclude the end '>' to be able to write attributes if needed.
                output.write(HTML_TAG_OPEN_LIST_ORDERED, sizeof(HTML_TAG_OPEN_LIST_ORDERED) - 2);

                // Start.
                if(node.meta.listStart != 1) {
                    output.write(" ", sizeof(" ") - 1);

                    output.write(HTML_ATTR_OL_START, sizeof(HTML_ATTR_OL_START) - 1);
                    output.write(HTML_ATTR_ASSIGNMENT, sizeof(HTML_ATTR_ASSIGNMENT) - 1);

                    output.adjust_capacity(LIMITS_UINT32_DIGITS);

                    output.set_size(output.get_size() + charlib::num_to_buffer(output.get_data() + output.get_size(), node.meta.listStart));
                }

                // Type.
                if(node.meta.listType != ListType::ARABIC) {
                    output.write(" ", sizeof(" ") - 1);

                    output.write(HTML_ATTR_OL_TYPE, sizeof(HTML_ATTR_OL_TYPE) - 1);
                    output.write(HTML_ATTR_ASSIGNMENT, sizeof(HTML_ATTR_ASSIGNMENT) - 1);

                    switch(node.meta.listType) {
                        case ListType::ROMAN_LOWER:
                            output.write(HTML_ATTR_OL_TYPE_ROMAN_LOWER, sizeof(HTML_ATTR_OL_TYPE_ROMAN_LOWER) - 1);
                            break;
                        case ListType::ROMAN_UPPER:
                            output.write(HTML_ATTR_OL_TYPE_ROMAN_UPPER, sizeof(HTML_ATTR_OL_TYPE_ROMAN_UPPER) - 1);
                            break;
                        case ListType::LETTER_LOWER:
                            output.write(HTML_ATTR_OL_TYPE_LETTER_LOWER, sizeof(HTML_ATTR_OL_TYPE_LETTER_LOWER) - 1);
                            break;
                        case ListType::LETTER_UPPER:
                            output.write(HTML_ATTR_OL_TYPE_LETTER_UPPER, sizeof(HTML_ATTR_OL_TYPE_LETTER_UPPER) - 1);
                            break;
                        default:
                            break;
                    }
                }

                // Reversed.
                if(node.meta.listReversed) {
                    output.write(" ", sizeof(" ") - 1);

                    output.write(HTML_ATTR_OL_REVERSED, sizeof(HTML_ATTR_OL_REVERSED) - 1);
                }

                // Write the ending '>' that was excluded.
                output.write(&HTML_TAG_OPEN_LIST_ORDERED[sizeof(HTML_TAG_OPEN_LIST_ORDERED) - 2], sizeof(char));

                render_children(node, output);
                output.write(HTML_TAG_CLOSE_LIST_ORDERED, sizeof(HTML_TAG_CLOSE_LIST_ORDERED) - 1);

                break;
            }
            case ASTNodeType::ORDERED_LIST_ELEMENT: {
                output.write(HTML_TAG_OPEN_LIST_ELEMENT, sizeof(HTML_TAG_OPEN_LIST_ELEMENT) - 1);
                render_children(node, output);
                output.write(HTML_TAG_CLOSE_LIST_ELEMENT, sizeof(HTML_TAG_CLOSE_LIST_ELEMENT) - 1);

                break;
            }
            case ASTNodeType::RAW_TEXT: {
                output.write(node.start, node.end - node.start);

                break;
            }
            case ASTNodeType::TEXT: {
                size_t size = node.end - node.start;
                size_t i = 0;

                while(i < size) {
                    render_char(node.start, i, size, output);
                }

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

    void Renderer::render_char(const char* start, size_t& index, size_t size, UnmanagedBuffer<char>& output) noexcept {
        #define TRANSLATE_CHAR(c) output.write(c, sizeof(c) - 1)

        switch(start[index]) {
            case '<':
                TRANSLATE_CHAR(HTML_ENTITY_LT);
                ++index;
                break;
            case '>':
                TRANSLATE_CHAR(HTML_ENTITY_GT);
                ++index;
                break;
            case '&': {
                size_t backupIndex = index;

                ++index;

                // Entities like &#1234;
                if(index < size && start[index] == HTML_ENTITY_NUM_START)
                    ++index;

                while(index < size && charlib::is_alphanum(start[index]))
                    ++index;

                // Don't translate &something; to &amp;something;
                if(index < size && start[index] == HTML_ENTITY_END) {
                    // Write '&'
                    output.write(start + backupIndex, sizeof(char));
                } else {
                    TRANSLATE_CHAR(HTML_ENTITY_AMP);
                }

                index = backupIndex + 1;
                break;
            }
            default: {
                output.write(start + index, sizeof(char));
                ++index;
                break;
            }
        }

        #undef TRANSLATE_CHAR
    }
}