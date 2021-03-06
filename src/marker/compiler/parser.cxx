#include "parser.hxx"
#include "../def/marker.dxx"
#include "../data/unmanaged_buffer.hxx"
#include "../lib/charlib.hxx"

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
                    indentStart = srcIndex;

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

                    REWIND(indentStart);

                    // Normal paragraph.
                    goto _block_paragraph; // Treat this block as a paragraph (see below).
                }
                case MARKER_LIST_UNORDERED: {
                    AST list;
                    size_t childIndent;

                    do {
                        childIndent = depth + indent + 1; // The list marker has length 1.

                        switch(NEXT) {
                            case '\r':
                            case '\n':
                                indentStart = srcIndex + 2;
                            case '\t':
                            case ' ':
                                break;
                            case '\0':
                                list.emplace_back(ASTNodeType::UNORDERED_LIST_ELEMENT, AST(), start);

                                return ASTNode(ASTNodeType::UNORDERED_LIST, list, start);
                            default:
                                goto _block_paragraph; // Treat this block as a paragraph (see below)
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
                            list.emplace_back(ASTNodeType::UNORDERED_LIST_ELEMENT, AST(), start);

                            return ASTNode(ASTNodeType::UNORDERED_LIST, list, start);
                        }

                        if(CURRENT != '\r' && CURRENT != '\n') {
                            ASTNode listElement(ASTNodeType::UNORDERED_LIST_ELEMENT);

                            // Process children.
                            do {
                                listElement.children.emplace_back(consume_block(childIndent));

                                if(CURRENT == '\0') {
                                    list.emplace_back(listElement);
                                    return ASTNode(ASTNodeType::UNORDERED_LIST, list, start);
                                }

                                indent = 0;
                                indentStart = srcIndex;

                                // Process indent (spaces, tabs, and line endings).
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

                                if(indent < childIndent) {
                                    break;
                                }
                            } while(true);

                            list.emplace_back(listElement);
                        } else {
                            list.emplace_back(ASTNodeType::UNORDERED_LIST_ELEMENT, AST(), start);

                            // Skip line endings.
                            do {
                                ADVANCE();
                            } while(CURRENT == '\r' || CURRENT == '\n');
                        }

                        if(CURRENT != MARKER_LIST_UNORDERED || indent < depth) {
                            REWIND(indentStart);
                            return ASTNode(ASTNodeType::UNORDERED_LIST, list, start);
                        }
                    } while(true);
                }
                default: {
                    // Ordered list.
                    ASTNode list;
                    size_t childIndent;
                    size_t length;

                    if(!consume_ordered_list(list.meta.listStart, list.meta.listType, length)) {
                        REWIND(indentStart);
                        goto _block_paragraph;
                    }

                    list.start = start;
                    list.type =  ASTNodeType::ORDERED_LIST;
                    list.meta.listReversed = false;

                    // Check the second element number. If it's smaller than the first, mark the list as reversed.
                    bool isSecondElement = true;

                    do {
                        childIndent = depth + indent + length;

                        switch(CURRENT) {
                            case '\r':
                            case '\n':
                                indentStart = srcIndex + 1; // Skip the character
                            case '\t':
                            case ' ':
                                break;
                            case '\0':
                                list.children.emplace_back(ASTNodeType::ORDERED_LIST_ELEMENT, AST(), start);

                                return list;
                            default:
                                REWIND(indentStart);
                                goto _block_paragraph; // Treat this block as a paragraph (see below)
                        }

                        while(true) {
                            if(CURRENT == ' ')
                                childIndent += 1;
                            else if(CURRENT == '\t')
                                childIndent += 4;
                            else break;

                            ADVANCE();
                        }

                        if(CURRENT == '\0') {
                            list.children.emplace_back(ASTNodeType::ORDERED_LIST_ELEMENT, AST(), start);

                            return list;
                        }

                        if(CURRENT != '\r' && CURRENT != '\n') {
                            ASTNode listElement(ASTNodeType::ORDERED_LIST_ELEMENT);

                            // Process children.
                            do {
                                listElement.children.emplace_back(consume_block(childIndent));

                                if(CURRENT == '\0') {
                                    list.children.emplace_back(listElement);
                                    return list;
                                }

                                indent = 0;
                                indentStart = srcIndex;

                                // Process indent (spaces, tabs, and line endings).
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
                                            goto _end_ordered_list_indent_loop;
                                    }

                                    ADVANCE();
                                }

                            _end_ordered_list_indent_loop:

                                if(indent < childIndent) {
                                    break;
                                }
                            } while(true);

                            list.children.emplace_back(listElement);
                        } else {
                            list.children.emplace_back(ASTNodeType::ORDERED_LIST_ELEMENT, AST(), start);

                            // Skip line endings.
                            do {
                                ADVANCE();
                            } while(CURRENT == '\r' || CURRENT == '\n');
                        }

                        uint32_t listStart;
                        ListType listType;

                        if(indent < depth || !consume_ordered_list(listStart, listType, length)) {
                            REWIND(indentStart);
                            return list;
                        }

                        if(listType != list.meta.listType) {
                            REWIND(indentStart);
                            return list;
                        }

                        // Mark the list as reversed if the second element has a smaller number.
                        if(isSecondElement) {
                            isSecondElement = false;

                            if(listStart < list.meta.listStart)
                                list.meta.listReversed = true;
                        }
                    } while(true);
                }

                // Still part of the default case.
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

                    if(!multiline || peek_block()) {
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
                        default: // Treat the escape character like a normal character.
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


    bool Parser::consume_ordered_list(uint32_t& listStart, ListType& listType, size_t& length) noexcept {
        size_t indexBackup = srcIndex;
        length = 0;

        if(charlib::is_lower_alpha(CURRENT)) {
            do {
                ++length;
                ADVANCE();
            } while(charlib::is_lower_alpha(CURRENT));

            if(CURRENT != MARKER_LIST_ORDERED) {
                REWIND(indexBackup);
                return false;
            }

            ADVANCE();
            ++length;

            if(charlib::roman_parse(src + indexBackup, length - 1, listStart)) {
                listType = ListType::ROMAN_LOWER;
                return true;
            }

            if(charlib::alpha_parse(src + indexBackup, length - 1, listStart)) {
                listType = ListType::LETTER_LOWER;
                return true;
            }
        } else if(charlib::is_upper_alpha(CURRENT)) {
            do {
                ++length;
                ADVANCE();
            } while(charlib::is_upper_alpha(CURRENT));

            if(CURRENT != MARKER_LIST_ORDERED) {
                REWIND(indexBackup);
                return false;
            }

            ADVANCE();
            ++length;

            if(charlib::roman_parse(src + indexBackup, length - 1, listStart)) {
                listType = ListType::ROMAN_UPPER;
                return true;
            }

            if(charlib::alpha_parse(src + indexBackup, length - 1, listStart)) {
                listType = ListType::LETTER_UPPER;
                return true;
            }
        } else if(charlib::is_digit(CURRENT)) {
            do {
                ++length;
                ADVANCE();
            } while(charlib::is_digit(CURRENT));

            if(CURRENT != MARKER_LIST_ORDERED) {
                REWIND(indexBackup);
                return false;
            }

            ADVANCE();
            ++length;

            if(charlib::num_parse(src + indexBackup, length - 1, listStart)) {
                listType = ListType::ARABIC;
                return true;
            }
        }

        return false;
    }

    bool Parser::consume_inline_modifier(InlineParser& inlineParser) noexcept {
        InlineStackInfo modInfo { };

        modInfo.start = &CURRENT;

        bool leaveAsRawText = false;

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
                leaveAsRawText = true; // Tags must have a RAW_TEXT child.
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
            consume_inline_modifier_end(inlineParser, modInfo, leaveAsRawText, startInfoIt);

            return true; // true - a modifier end was found.
        }
    }

    void Parser::consume_inline_modifier_start(InlineParser& inlineParser, InlineStackInfo& modInfo) noexcept {
        modInfo.outputIndex = inlineParser.ast.size();
        inlineParser.stack.push_back(modInfo);
    }

    void Parser::consume_inline_modifier_end(InlineParser& inlineParser, InlineStackInfo& modInfo, bool leaveAsRawText, decltype(inlineParser.stack)::iterator startInfoIt) noexcept {
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
                    if(splitText.start < splitText.end && !leaveAsRawText)
                        inlineNode.children.push_back(splitText);
                }
            } else if(inlineParser.ast[modifierIndex].start == startInfo.start) {
                // Text starts with the modifier.
                inlineParser.ast[modifierIndex].start = startInfo.start + 1;

                if(!leaveAsRawText)
                    inlineNode.children.push_back(inlineParser.ast[modifierIndex]);
            } else {
                if(!leaveAsRawText)
                    inlineNode.children.push_back(inlineParser.ast[modifierIndex]);
            }

            ++modifierIndex;
        }

        if(!leaveAsRawText) {
            // Make the output AST elements be children of this modifier.
            for(; modifierIndex < inlineParser.ast.size(); ++modifierIndex) {
                // Tags need only text children.
                inlineNode.children.push_back(inlineParser.ast[modifierIndex]);
            }
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

                    splitText.start = startInfo.start + 1;
                    splitText.end = inlineParser.text.end;

                    inlineParser.text.end = startInfo.start;

                    inlineParser.ast.push_back(inlineParser.text);

                    // Right text is not empty.
                    if(splitText.start < splitText.end && !leaveAsRawText)
                        inlineNode.children.push_back(splitText);
                }
            } else if(inlineParser.text.start == startInfo.start) {
                // Text starts with the modifier.
                inlineParser.text.start = startInfo.start + 1;

                if(!leaveAsRawText)
                    inlineNode.children.push_back(inlineParser.text);
            } else {
                if(!leaveAsRawText)
                    inlineNode.children.push_back(inlineParser.text);
            }
        }

        if(leaveAsRawText) {
            // Write the raw text.
            inlineNode.children.push_back(ASTNode(ASTNodeType::RAW_TEXT, inlineNode.start + 1, inlineNode.end - 1));
        }

        // Push the modifier node.
        inlineParser.ast.push_back(inlineNode);

        inlineParser.text.start = nullptr;

        ADVANCE();
    }

    bool Parser::peek_block() noexcept {
        size_t indexBackup = srcIndex;

        while(CURRENT == ' ' || CURRENT == '\t') {
            ADVANCE();
        }

        switch(CURRENT) {
            case '\r':
            case '\n':
            case '\0':
                REWIND(indexBackup);
                return false;
            case MARKER_HEADING: {
                size_t headingLevel = 0;

                do {
                    ++headingLevel;
                    ADVANCE();
                } while(CURRENT == MARKER_HEADING);

                if(headingLevel <= 6 && (CURRENT == ' ' || CURRENT == '\t')) {
                    REWIND(indexBackup);
                    return true;
                }

                REWIND(indexBackup);
                return false;
            }
            case MARKER_LIST_UNORDERED: {
                switch(NEXT) {
                    case ' ':
                    case '\t':
                    case '\r':
                    case '\n':
                    case '\0':
                        REWIND(indexBackup);
                        return true;
                    default:
                        REWIND(indexBackup);
                        return false;
                }
            }
            default: {
                // Ordered list.
                uint32_t listStart;
                ListType listType;
                size_t length;

                if(consume_ordered_list(listStart, listType, length)) {
                    REWIND(indexBackup);
                    return true;
                }

                // Paragraph.
                REWIND(indexBackup);
                return false;
            }
        }
    }
}

#undef REWIND
#undef STEP_BACK
#undef ADVANCE
#undef NEXT
#undef CURRENT
#undef PEEK
#undef AT_END