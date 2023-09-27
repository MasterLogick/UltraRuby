#ifndef ULTRA_RUBY_STATEMENTTYPE_H
#define ULTRA_RUBY_STATEMENTTYPE_H

namespace UltraRuby {
namespace AST {

enum StatementType {
    STMT_UNKNOWN,

    STMT_BIN_OP,
    STMT_UN_OP,
    STMT_ASSIGNMENT,

    STMT_CALL,

    STMT_FLOAT_CONST,
    STMT_INT_CONST,
    STMT_STRING,
    STMT_SYMBOL,

    STMT_ARRAY,
    STMT_HASH,

    STMT_VARIABLE,

    STMT_CLASS_DEF,
    STMT_MODULE_DEF,
    STMT_FUNC_DEF,
    STMT_CLASS_INSTANCE_DEF,

    STMT_BLOCK,
    STMT_EXCEPTIONAL_BLOCK,

    STMT_IF,
    STMT_WHILE,
    STMT_CASE,
    STMT_FOR,

    STMT_BOOL_CONST,

    STMT_RETURN,
    STMT_NEXT,
    STMT_BREAK,
    STMT_REDO,
    STMT_RETRY,
    STMT_YIELD,
    STMT_YIELD_SELF,


};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_STATEMENTTYPE_H
