#ifndef ULTRA_RUBY_AST_OPERATIONTYPE_H
#define ULTRA_RUBY_AST_OPERATIONTYPE_H

namespace UltraRuby {
namespace AST {

enum OperationType {
    OP_UNKNOWN,

    BIN_OP_LESS,
    BIN_OP_LESS_EQUAL,
    BIN_OP_EQUAL,
    BIN_OP_CASE_EQUAL,
    BIN_OP_GREATER_EQUAL,
    BIN_OP_GREATER,
    BIN_OP_THREE_WAY_COMPARE,

    BIN_OP_BIN_OR,
    BIN_OP_OR,
    BIN_OP_BIN_AND,
    BIN_OP_AND,
    BIN_OP_XOR,
    BIN_OP_LEFT_SHIFT,
    BIN_OP_RIGHT_SHIFT,
    BIN_OP_ADD,
    BIN_OP_SUBTRACT,
    BIN_OP_MULTIPLY,
    BIN_OP_DIVIDE,
    BIN_OP_MOD,
    BIN_OP_POWER,

    BIN_OP_ASSIGN,
    BIN_OP_BIN_OR_ASSIGN,
    BIN_OP_OR_ASSIGN,
    BIN_OP_BIN_AND_ASSIGN,
    BIN_OP_AND_ASSIGN,
    BIN_OP_XOR_ASSIGN,
    BIN_OP_LEFT_SHIFT_ASSIGN,
    BIN_OP_RIGHT_SHIFT_ASSIGN,
    BIN_OP_ADD_ASSIGN,
    BIN_OP_SUBTRACT_ASSIGN,
    BIN_OP_MULTIPLY_ASSIGN,
    BIN_OP_DIVIDE_ASSIGN,
    BIN_OP_MOD_ASSIGN,
    BIN_OP_POWER_ASSIGN,
    BIN_OP_QUESTION,

    UN_OP_NOT,
    UN_OP_POSITIVE,
    UN_OP_NEGATIVE,
    UN_OP_BIN_NEGATION,
    UN_OP_LIST_UNWRAP,

    LEX_OP_PLUS,
    LEX_OP_MINUS,
    LEX_OP_STAR,
};

constexpr OperationType BIN_OP_REGION_START = BIN_OP_LESS;
constexpr OperationType BIN_OP_ASSIGNMENT_START = BIN_OP_ASSIGN;
constexpr OperationType BIN_OP_REGION_END = BIN_OP_QUESTION;
constexpr OperationType UN_OP_REGION_START = UN_OP_NOT;
constexpr OperationType UN_OP_REGION_END = UN_OP_LIST_UNWRAP;
constexpr OperationType LEXER_OP_REGION_START = LEX_OP_PLUS;
constexpr OperationType LEXER_OP_REGION_END = LEX_OP_STAR;

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_OPERATIONTYPE_H
