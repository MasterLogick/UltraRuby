#ifndef ULTRA_RUBY_PARSER_PARSER_H
#define ULTRA_RUBY_PARSER_PARSER_H

#include <map>
#include "../lexer/Lexer.h"
#include "../ast/ASTClassDecls.h"
#include "../ast/FuncDefArg.h"

namespace UltraRuby {
namespace Parser {

class Parser {
public:
    explicit Parser(Lexer::TokenQueue *queue) : queue(queue), error(false) {
        currentLexerToken = queue->getNextToken();
    }

    /**
     * program ::= top_stmt
     *         ::= top_stmt program
     * @return
     */
    AST::Block *parseProgram();

    bool hasErrors() {
        return error;
    }

private:
    /**
     * top_stmt ::= stmt
     *          ::= BEGIN { program }
     * @return
     */
    AST::Statement *parseTopStatement();

    /**
     * stmt ::= uncontrolled_stmt
     *      ::= stmt control_modifier uncontrolled_stmt
     *
     * control_modifier ::= k_if
     *                  ::= k_unless
     *                  ::= k_while
     *                  ::= k_until
     * @return
     */
    AST::Statement *parseStatement(int opPrecedence = 0);

    /**
     * uncontrolled_stmt ::= primary
     *                   ::= primary bin_op uncontrolled_stmt
     * @return
     */
    AST::Statement *parseUncontrolledStatement(int opPrecedence = 0);

    /**
     * primary ::= primary_object
     *         ::= primary . identifier call_args
     *         ::= primary . call_args
     *         ::= primary :: name
     * @return
     */
    AST::Statement *parsePrimary();

    /**
     * primary_object ::= string
     *                ::= int
     *                ::= float
     *                ::= k_true
     *                ::= k_false
     *                ::= k_self
     *                ::= : identifier
     *                ::= k_begin comp_statement k_end
     *                ::= ( comp_statement )
     *                ::= [ array
     *                ::= { hash
     *                ::= k_return call_args
     *                ::= k_next call_args
     *                ::= k_break call_args
     *                ::= k_redo
     *                ::= k_retry
     *                ::= k_yield call_args
     *                ::= k_yield_self
     *                ::= k_if stmt k_then_opt comp_statement if_branches
     *                ::= k_unless stmt k_then_opt comp_statement k_end
     *                ::= k_while stmt k_do_opt comp_statement k_end
     *                ::= k_until stmt k_do_opt comp_statement k_end
     *                ::= k_case stmt case_body k_end
     *                ::= k_for stmt k_in stmt k_do_opt comp_statement k_end
     *                ::= class definitionName body_stmt k_end
     *                ::= class definitionName < stmt body_stmt k_end
     *                ::= class << stmt body_stmt k_end
     *                ::= k_module definitionName body_stmt k_end
     *                ::= def func_name func_def_args body_stmt k_end
     *                ::= def func_name ( func_def_args ) body_stmt k_end
     *                ::= un_op primary
     *                ::= @ identifier
     *                ::= name
     *                ::= :: name
     *                ::= name call_args
     *                ::= :: name call_args
     * @return
     */
    AST::Statement *parsePrimaryObject();

    bool primaryTest();

    /**
     * comp_stmt ::= stmt
     *           ::= stmt terms comp_stmt
     * terms ::= terms ;
     *       ::= terms \\n
     *       ::= ;
     *       ::= \\n
     * @return
     */
    AST::Block *parseCompStatement();

    /**
     * array ::= ]
     *       ::= array_body ]
     * array_body ::= stmt
     *            ::= stmt, array_body
     * @return
     */
    AST::Statement *parseArray();

    /**
     * map ::= }
     *     ::= map_body }
     * map_body ::= el
     *          ::= el, map_body
     * el ::= text : stmt
     *    ::= stmt => stmt
     * @return
     */
    AST::Statement *parseHash();

    /**
     * if_branches ::= elsif stmt then_opt comp_statement if_branches
     *             ::= else comp_statement end
     * @return
     */
    AST::Statement *parseIfBranches(AST::Statement *cond, AST::Statement *trueBranch);

    /**
     * call_args ::= call_args_inside
     *           ::= ( call_args_inside ) block_opt
     *           ::= [ call_args_inside ] block_opt
     *           ::= block_opt
     * call_args_inside ::= arg, call_args_inside
     * arg ::= statement
     *     ::= identifier : statement
     * @return
     */
    std::pair<AST::CallArgs *, bool> parseCallArgs();

    /**
     * block ::= { block_body }
     *       ::= do block_body end
     * block_body ::= comp_statement
     *            ::= | func_def_args | comp_statement
     * @return
     */
    AST::FunctionDef *parseBlock();

    /**
     * func_def_args ::= arg
     *               ::= arg, func_def_args
     * arg ::= identifier
     *     ::= identifier = statement
     *     ::= identifier : statement
     * @return
     */
    bool parseFuncDefArgs(std::vector<AST::FuncDefArg *> &args, bool greedy);

    /**
     * name ::= identifier
     *      ::= identifier ?
     *      ::= identifier !
     *      ::= identifier =
     * @return
     */
    std::string parseName(bool withEqSign);


    Lexer::TokenType nextLexerToken(bool skipSpaces = false);

    Lexer::TokenType prevLexerToken();

    Lexer::TokenType rewindTo(int ptr);

    void logError(const char *err);

    AST::Statement *parseBinOpRight(AST::Statement *left, int precedence = 0);

    AST::OperationType convertToBinOp(AST::OperationType type);

    AST::OperationType convertToUnOp(AST::OperationType type);

    int getOpPrecedence(AST::OperationType);

    void skipTerms();

    void skipSpaces();

    bool isATerm(Lexer::TokenType token);

    std::string parseFunctionName();

    AST::Statement *parseFunctionDef();

    bool parseFunctionDefHeader(std::string *functionName, AST::Statement **singleton);

    Lexer::TokenQueue *queue;
    Lexer::TokenType currentLexerToken;
    bool error;

    bool testName(bool withEqSign);
};

} // UltraRuby
} // Parser

#endif //ULTRA_RUBY_PARSER_PARSER_H
