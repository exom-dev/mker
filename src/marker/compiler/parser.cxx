#include "parser.hxx"
#include "../def/marker.dxx"
#include "../data/unmanaged_buffer.hxx"

#include <cstddef>
#include <stack>

#define AT_END(index) (src[index] == '\0')
#define PEEK(index)   (src[srcIndex + index])
#define CURRENT       (src[srcIndex])
#define NEXT          (src[srcIndex + 1])
#define ADVANCE()     (++srcIndex)
#define STEP_BACK()   (--srcIndex)
#define REWIND(index) (srcIndex = index)

namespace mker {
    Parser::Parser(const char* source) noexcept {
        src = source;

        srcIndex = 0;
    }

    AST Parser::parse() noexcept {
        AST output;

        while(CURRENT != '\0') {
            output.emplace_back(std::move(consume_block(0)));
        }

        return output;
    }

    ASTNode Parser::consume_block(size_t depth) noexcept {
        while(true) {
            size_t indent = 0;
            size_t indentStart = srcIndex;

            // Advance through spaces and tabs.
            while(true) {
                if(CURRENT == ' ')
                    indent += 1;
                else if(CURRENT == '\t')
                    indent += 4;
                else break;

                ADVANCE();
            }

            switch(CURRENT) {
                case '\r':
                    if(NEXT == '\n')
                        ADVANCE();
                    // Fallthrough. This setup consumes \r\n, \r, and \n, all in one.
                case '\n':
                    ADVANCE();

                    indent = 0;
                    indentStart = srcIndex;

                    continue;
                case '\0':
                    break;
            }

            /*if(depth == 0 && indent >= 4) {
                exit(1);
                //consume_block(src, srcIndex, output);
            }*/

            const char* start = src + srcIndex;

            switch(CURRENT) {
                case MARKER_HEADING: {
                    size_t headingLevel = 0;
                    size_t indexBackup = srcIndex;

                    do {
                        ++headingLevel;
                        ADVANCE();
                    } while(CURRENT == MARKER_HEADING);

                    if(headingLevel <= 6 && (CURRENT == ' ' || CURRENT == '\t')) {
                        do {
                            ADVANCE();
                        } while(CURRENT == ' ' ||  CURRENT == '\t');

                        return ASTNode(
                            static_cast<ASTNodeType>(
                                static_cast<unsigned int>(ASTNodeType::HEADING1) + headingLevel - 1
                            ),
                            consume_paragraph(false),
                            start
                        );
                    }

                    REWIND(indexBackup);

                    // Normal paragraph.
                    goto _block_paragraph; // Treat this block as a paragraph (see below).
                }
                case MARKER_LIST_UNORDERED: {
                    AST list;
                    size_t childIndent; // = indent + 1;

                    do {
                        childIndent = indent + 1; // The list marker has length 1.

                        switch(NEXT) {
                            case ' ':
                            case '\t':
                            case '\r':
                            case '\n':
                                break;
                            case '\0':
                                list.emplace_back(ASTNodeType::LIST_ELEMENT, AST(), start);

                                return ASTNode(ASTNodeType::LIST, list, start);
                            default:
                                goto _block_paragraph; // Treat this block as a paragraph (see below)..
                        }

                        ADVANCE();

                        while(true) {
                            if(CURRENT == ' ')
                                childIndent += 1;
                            else if(CURRENT == '\t')
                                childIndent += 4;
                            else break;

                            ADVANCE();
                        }

                        if(CURRENT == '\0') {
                            list.emplace_back(ASTNodeType::LIST_ELEMENT, AST(), start);

                            return ASTNode(ASTNodeType::LIST, list, start);
                        }

                        if(CURRENT != '\r' && CURRENT != '\n') {
                            ASTNode listElement(ASTNodeType::LIST_ELEMENT);

                            do {
                                listElement.children.emplace_back(consume_block(childIndent));

                                if(CURRENT == '\0') {
                                    list.emplace_back(listElement);
                                    return ASTNode(ASTNodeType::LIST, list, start);
                                }

                                indent = 0;
                                indentStart = srcIndex;

                                while(true) {
                                    switch(CURRENT) {
                                        case ' ':
                                            ++indent;
                                            break;
                                        case '\t':
                                            indent += 4;
                                            break;
                                        case '\r':
                                            if(NEXT == '\n')
                                                ADVANCE();

                                            // Fallthrough.
                                        case '\n':
                                            indent = 0;
                                            indentStart = srcIndex;

                                            break;
                                        default:
                                            goto _end_list_indent_loop;
                                    }

                                    ADVANCE();
                                }

                                _end_list_indent_loop:

                                if(indent < childIndent)
                                    break;
                            } while(true);

                            list.emplace_back(listElement);
                        } else {
                            list.emplace_back(ASTNodeType::LIST_ELEMENT, AST(), start);
                        }

                        if(CURRENT != MARKER_LIST_UNORDERED) {
                            REWIND(indentStart);
                            return ASTNode(ASTNodeType::LIST, list, start);
                        }
                    } while(true);
                }
                default:
                _block_paragraph:
                    return ASTNode(ASTNodeType::PARAGRAPH, consume_paragraph(true), start);
            }
        }
    }

    AST Parser::consume_paragraph(bool multiline) noexcept {
        InlineParser inlineParser;

        inlineParser.text = ASTNode(ASTNodeType::TEXT);

        while(true) {
            switch(CURRENT) {
                case '\r':
                    if(NEXT == '\n')
                        ADVANCE();
                    // Fallthrough. New line detected.
                case '\n': {
                    ADVANCE();

                    if(!multiline) {
                        if(inlineParser.text.start != nullptr) {
                            inlineParser.ast.emplace_back(inlineParser.text);
                        }

                        return inlineParser.ast;
                    }

                    while(CURRENT == ' ' || CURRENT == '\t')
                        ADVANCE();

                    switch(CURRENT) {
                        case '\r':
                            if(NEXT == '\n')
                                ADVANCE();
                            // Fallthrough. Blank line detected.
                        case '\n':
                            ADVANCE();

                            if(inlineParser.text.start != nullptr) {
                                inlineParser.ast.emplace_back(inlineParser.text);
                            }

                            return inlineParser.ast; // Double line endings -> the paragraph ended.
                        case '\0':
                            continue;
                        default:
                            if(inlineParser.text.start != nullptr) {
                                inlineParser.ast.emplace_back(inlineParser.text);
                                inlineParser.text.start = nullptr;
                            }

                            inlineParser.ast.emplace_back(ASTNodeType::SOFTBREAK, &CURRENT - 1, &CURRENT);

                            continue;
                    }
                }
                case MARKER_EMPHASIS:
                case MARKER_STRONG:
                case MARKER_UNDERLINE:
                case MARKER_STRIKETHROUGH:
                case MARKER_TAG_OPEN:
                case MARKER_TAG_CLOSE: {
                    if(consume_inline_modifier(inlineParser))
                        break;

                    // Fallthrough if a modifier start was found.
                    goto _paragraph_normal_char;
                }
                case MARKER_ESCAPE: {
                    switch(NEXT) {
                        case MARKER_EMPHASIS:
                        case MARKER_STRONG:
                        case MARKER_UNDERLINE:
                        case MARKER_STRIKETHROUGH:
                        case MARKER_TAG_OPEN:
                        case MARKER_TAG_CLOSE:
                        case MARKER_ESCAPE: {
                            if(inlineParser.text.start != nullptr) {
                                inlineParser.ast.emplace_back(inlineParser.text);

                                inlineParser.text.start = nullptr;
                            }

                            ADVANCE(); // Skip the escape character.
                            // Fallthrough.
                        }
                        default: ; // Treat the escape character like a normal character.
                        // Fallthrough.
                        goto _paragraph_normal_char;
                    }
                }
                case '\0': {
                    if(inlineParser.text.start != nullptr) {
                        inlineParser.ast.emplace_back(inlineParser.text);
                    }

                    return inlineParser.ast;
                }
                _paragraph_normal_char:
                default: {
                    if(inlineParser.text.start == nullptr) {
                        inlineParser.text.start = &CURRENT;
                        inlineParser.text.end = inlineParser.text.start;
                    }

                    ADVANCE();
                    ++inlineParser.text.end;
                    break;
                }
            }
        }
    }

    bool Parser::consume_inline_modifier(InlineParser& inlineParser) noexcept {
        InlineStackInfo modInfo { };

        modInfo.start = &CURRENT;

        bool forceTextChildren = false;

        switch(CURRENT) {
            case MARKER_EMPHASIS:
                modInfo.type = ASTNodeType::EMPHASIS;
                break;
            case MARKER_STRONG:
                modInfo.type = ASTNodeType::STRONG;
                break;
            case MARKER_UNDERLINE:
                modInfo.type = ASTNodeType::UNDERLINE;
                break;
            case MARKER_STRIKETHROUGH:
                modInfo.type = ASTNodeType::STRIKETHROUGH;
                break;
            case MARKER_TAG_OPEN:
            case MARKER_TAG_CLOSE:
                modInfo.type = ASTNodeType::TAG;
                forceTextChildren = true; // Tags must have Text children.
                break;
            default:
                return false;
        }

        auto startInfoIt = inlineParser.stack.end();

        if(!inlineParser.stack.empty()) {
            // Find the modifier start on the stack.
            for (size_t i = inlineParser.stack.size(); i > 0; --i) {
                if(inlineParser.stack[i - 1].type == modInfo.type) {
                    startInfoIt = inlineParser.stack.begin() + i - 1;
                    break;
                }
            }
        }

        if(startInfoIt == inlineParser.stack.end()) {
            if(CURRENT == MARKER_TAG_CLOSE) // A closing tag marker can't start a tag.
                return false;

            // Modifier start wasn't found. Create one.
            consume_inline_modifier_start(inlineParser, modInfo);

            return false; // false - a modifier end wasn't found.
        } else {
            if(CURRENT == MARKER_TAG_OPEN) // An opening tag marker can't end a tag.
                return false;

            // Modifier start was found. End it.
            consume_inline_modifier_end(inlineParser, modInfo, forceTextChildren, startInfoIt);

            return true; // true - a modifier end was found.
        }
    }

    void Parser::consume_inline_modifier_start(InlineParser& inlineParser, InlineStackInfo& modInfo) noexcept {
        modInfo.outputIndex = inlineParser.ast.size();
        inlineParser.stack.push_back(modInfo);
    }

    void Parser::consume_inline_modifier_end(InlineParser& inlineParser, InlineStackInfo& modInfo, bool forceTextChildren, decltype(inlineParser.stack)::iterator startInfoIt) noexcept {
        ASTNode inlineNode(modInfo.type);
        InlineStackInfo startInfo = *startInfoIt;

        inlineNode.start = startInfo.start;
        inlineNode.end = &CURRENT + 1;

        size_t eraseStartIndex = startInfo.outputIndex;

        // If other inline elements are between the modifier markers,
        // mark them as text by erasing them from the inline stack.
        inlineParser.stack.erase(startInfoIt + 1, inlineParser.stack.end());

        size_t modifierIndex = startInfo.outputIndex;

        // Process the text on the same line that the modifier started on.
        if(modifierIndex < inlineParser.ast.size() && inlineParser.ast[modifierIndex].type == ASTNodeType::TEXT) {
            if(inlineParser.ast[modifierIndex].start < startInfo.start) {
                // Text starts before, finishes after. Split it into two.
                if(inlineParser.ast[modifierIndex].end > startInfo.start) {
                    ASTNode splitText(ASTNodeType::TEXT);

                    splitText.start = startInfo.start + 1;// + (startInfo.type == ASTNodeType::EMPHASIS);
                    splitText.end = inlineParser.ast[modifierIndex].end;

                    inlineParser.ast[modifierIndex].end = startInfo.start;
                    ++eraseStartIndex; // Don't delete this text, as it's not a child of the inline node.

                    // Right text is not empty.
                    if(splitText.start < splitText.end)
                        inlineNode.children.push_back(splitText);
                }
            } else if(inlineParser.ast[modifierIndex].start == startInfo.start) {
                // Text starts with the modifier.
                inlineParser.ast[modifierIndex].start = startInfo.start + 1;// + (startInfo.type == ASTNodeType::EMPHASIS);
                inlineNode.children.push_back(inlineParser.ast[modifierIndex]);
            } else {
                inlineNode.children.push_back(inlineParser.ast[modifierIndex]);
            }

            ++modifierIndex;
        }

        // Make the output AST elements be children of this modifier.
        for(; modifierIndex < inlineParser.ast.size(); ++modifierIndex) {
            // Tags need only text children.
            if(forceTextChildren)
                inlineNode.children.push_back(ASTNode(ASTNodeType::TEXT, inlineParser.ast[modifierIndex].start, inlineParser.ast[modifierIndex].end));
            else inlineNode.children.push_back(inlineParser.ast[modifierIndex]);
        }

        // Pop the modifier start (it's at the top because the others have been erased).
        inlineParser.stack.pop_back();

        // Erase all modifier children from the AST.
        inlineParser.ast.erase(inlineParser.ast.begin() + eraseStartIndex, inlineParser.ast.end());

        // Process the text on the current line.
        if(inlineParser.text.start != nullptr) {
            if(inlineParser.text.start < startInfo.start) {
                // Text starts before, finishes after. Split it into two.
                if(inlineParser.text.end > startInfo.start) {
                    ASTNode splitText(ASTNodeType::TEXT);

                    splitText.start = startInfo.start + 1;// + (startInfo.type == ASTNodeType::STRONG);
                    splitText.end = inlineParser.text.end;

                    inlineParser.text.end = startInfo.start;

                    inlineParser.ast.push_back(inlineParser.text);

                    // Right text is not empty.
                    if(splitText.start < splitText.end)
                        inlineNode.children.push_back(splitText);
                }
            } else if(inlineParser.text.start == startInfo.start) {
                // Text starts with the modifier.
                inlineParser.text.start = startInfo.start + 1;// + (startInfo.type == ASTNodeType::STRONG);
                inlineNode.children.push_back(inlineParser.text);
            } else {
                inlineNode.children.push_back(inlineParser.text);
            }
        }

        // Push the modifier node.
        inlineParser.ast.push_back(inlineNode);

        inlineParser.text.start = nullptr;

        ADVANCE();
    }
}

#undef REWIND
#undef STEP_BACK
#undef ADVANCE
#undef NEXT
#undef CURRENT
#undef PEEK
#undef AT_END