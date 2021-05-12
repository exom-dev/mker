#ifndef MARKER_DATA_AST_HXX_GUARD
#define MARKER_DATA_AST_HXX_GUARD

#include <vector>
#include <cstddef>
#include <cstdint>

struct ASTNode;
typedef std::vector<ASTNode> AST;

enum class ASTNodeType {
    NONE,

    // Blocks.
    PARAGRAPH,
    HEADING1,
    HEADING2,
    HEADING3,
    HEADING4,
    HEADING5,
    HEADING6,
    UNORDERED_LIST,
    UNORDERED_LIST_ELEMENT,
    ORDERED_LIST,
    ORDERED_LIST_ELEMENT,

    // Inline elements.
    RAW_TEXT,
    TEXT,
    EMPHASIS,
    STRONG,
    UNDERLINE,
    STRIKETHROUGH,
    TAG,
    SOFTBREAK
};

enum class ListType {
    ARABIC,
    ROMAN_LOWER,
    ROMAN_UPPER,
    LETTER_LOWER,
    LETTER_UPPER
};

struct ASTNode {
    ASTNodeType type;
    AST children;

    struct {
        uint32_t listStart;
        ListType listType;
        bool listReversed;
    } meta;

    ASTNode() { }

    ASTNode(ASTNodeType nodeType)
      : type(nodeType),
        start(nullptr) { }

    ASTNode(ASTNodeType nodeType, const char* nodeStart, const char* nodeEnd)
        : type(nodeType),
          start(nodeStart),
          end(nodeEnd) { }

    ASTNode(ASTNodeType nodeType, std::vector<ASTNode>&& nodeChildren, const char* nodeStart)
      : type(nodeType),
        children(std::move(nodeChildren)),
        start(nodeStart) { }

    ASTNode(ASTNodeType nodeType, const std::vector<ASTNode>& nodeChildren, const char* nodeStart)
        : type(nodeType),
          children(std::move(nodeChildren)),
          start(nodeStart) { }

    const char* start;
    const char* end;
};

#endif