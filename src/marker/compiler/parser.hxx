#ifndef MARKER_PARSER_HXX_GUARD
#define MARKER_PARSER_HXX_GUARD

#include "../data/ast.hxx"

#include <cstddef>

namespace mker {
    class Parser {
        struct InlineStackInfo {
            ASTNodeType type;
            size_t outputIndex; // The index in the AST where the marker
            const char* start;  // Where the marker starts in the source.
        };

        struct InlineParser {
            AST ast; // Output AST.
            std::vector<InlineStackInfo> stack; // Inline modifiers.
            ASTNode text; // Current text element.
        };

        private:
            const char* src;

            size_t srcIndex;

        public:
            explicit Parser(const char *source) noexcept;

            AST parse() noexcept;

        private:
            ASTNode consume_block                 (size_t depth)          noexcept;
            AST     consume_paragraph             (bool multiline)        noexcept;

            bool    consume_inline_modifier       (InlineParser& inlineParser) noexcept;
            void    consume_inline_modifier_start (InlineParser& inlineParser, InlineStackInfo& modInfo) noexcept;
            void    consume_inline_modifier_end   (InlineParser& inlineParser, InlineStackInfo& modInfo, bool forceTextChildren, decltype(inlineParser.stack)::iterator startInfoIt) noexcept;
    };
}

#endif
