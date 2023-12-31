#ifndef ULTRA_RUBY_LEXER_TOKENTYPE_H
#define ULTRA_RUBY_LEXER_TOKENTYPE_H

namespace UltraRuby::Lexer {
enum TokenType {
    TOK_ERROR = 0,

    TOK_SPACES,
    TOK_EOF,
    TOK_NEWLINE,

    TOK_IDENTIFIER,
    TOK_FLOAT,
    TOK_INTEGER,
    TOK_COMMON_STRING,
    TOK_RAW_STRING,

    TOK_BEGIN_UPPER,
    TOK_BEGIN,
    TOK_END_UPPER,
    TOK_END,
    TOK_ALIAS,
    TOK_AND,
    TOK_BREAK,
    TOK_CASE,
    TOK_CLASS,
    TOK_DEF,
    TOK_DEFINED,
    TOK_DO,
    TOK_ELSE,
    TOK_ELSIF,
    TOK_ENSURE,
    TOK_FALSE,
    TOK_FOR,
    TOK_IN,
    TOK_MODULE,
    TOK_NEXT,
    TOK_NIL,
    TOK_NOT,
    TOK_OR,
    TOK_REDO,
    TOK_RESCUE,
    TOK_RETRY,
    TOK_RETURN,
    TOK_SELF,
    TOK_SUPER,
    TOK_THEN,
    TOK_TRUE,
    TOK_UNDEF,
    TOK_WHEN,
    TOK_YIELD,
    TOK_YIELD_SELF,
    TOK_IF,
    TOK_UNLESS,
    TOK_WHILE,
    TOK_UNTIL,

    TOK_OP,

    TOK_COMMA,
    TOK_DOT,
    TOK_COLON,
    TOK_DOUBLE_COLON,
    TOK_SEMICOLON,
    TOK_AT_SIGN,
    TOK_HASH_ASSOC,

    /// token (
    TOK_PAREN_LEFT,
    /// token )
    TOK_PAREN_RIGHT,
    /// token [
    TOK_BRACKET_LEFT,
    /// token ]
    TOK_BRACKET_RIGHT,
    /// token {
    TOK_BRACE_LEFT,
    /// token }
    TOK_BRACE_RIGHT
};

const char *strTokenType(TokenType type);
}

#endif //ULTRA_RUBY_LEXER_TOKENTYPE_H
