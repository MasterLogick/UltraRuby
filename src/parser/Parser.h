#ifndef ULTRA_RUBY_PARSER_PARSER_H
#define ULTRA_RUBY_PARSER_PARSER_H

#include <map>
#include "../lexer/Lexer.h"
#include "../ast/ASTClassDecls.h"

namespace UltraRuby {
namespace Parser {

class Parser {
public:
    explicit Parser(Lexer::TokenQueue *queue)
            : queue(queue) {
        currentLexerToken = queue->getNextToken();
    }

    /**
     * program ::= top_stmt
     *         ::= top_stmt program
     * @return
     */
    std::unique_ptr<AST::Block> parseProgram();

private:
    /**
     * top_stmt ::= stmt
     *          ::= BEGIN { program }
     * @return
     */
    std::unique_ptr<AST::Statement> parseTopStatement();

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
    std::unique_ptr<AST::Statement> parseStatement(int opPrecedence = 0);

    /**
     * uncontrolled_stmt ::= primary
     *                   ::= primary bin_op uncontrolled_stmt
     * @return
     */
    std::unique_ptr<AST::Statement> parseUncontrolledStatement(int opPrecedence = 0);

    /**
     * primary ::= primary_object
     *         ::= primary . identifier call_args
     * @return
     */
    std::unique_ptr<AST::Statement> parsePrimary();

    /**
     * primary_object ::= string
     *         ::= int
     *         ::= float
     *         ::= k_true
     *         ::= k_false
     *         ::= k_self
     *         ::= : identifier
     *         ::= k_begin comp_statement k_end
     *         ::= ( comp_statement )
     *         ::= [ array
     *         ::= { hash
     *         ::= k_return call_args
     *         ::= k_next call_args
     *         ::= k_break call_args
     *         ::= k_redo
     *         ::= k_retry
     *         ::= k_yield call_args
     *         ::= k_yield_self
     *         ::= k_if stmt k_then_opt comp_statement if_branches
     *         ::= k_unless stmt k_then_opt comp_statement k_end
     *         ::= k_while stmt k_do_opt comp_statement k_end
     *         ::= k_until stmt k_do_opt comp_statement k_end
     *         ::= k_case stmt case_body k_end
     *         ::= k_for stmt k_in stmt k_do_opt comp_statement k_end
     *         ::= class variable superclass_opt body_stmt k_end
     *         ::= class << stmt body_stmt k_end
     *         ::= k_module variable body_stmt k_end
     *         ::= def func_name func_def_args body_stmt k_end
     *         ::= def func_name ( func_def_args ) body_stmt k_end
     *         ::= un_op primary
     *         ::= variable
     * @return
     */
    std::unique_ptr<AST::Statement> parsePrimaryObject();


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
    std::unique_ptr<AST::Block> parseCompStatement();

    /**
     * array ::= ]
     *       ::= array_body ]
     * array_body ::= stmt
     *            ::= stmt, array_body
     * @return
     */
    std::unique_ptr<AST::Statement> parseArray();

    /**
     * map ::= }
     *     ::= map_body }
     * map_body ::= el
     *          ::= el, map_body
     * el ::= text : stmt
     *    ::= stmt => stmt
     * @return
     */
    std::unique_ptr<AST::Statement> parseHash();

    /**
     * if_branches ::= elsif stmt then_opt comp_statement if_branches
     *             ::= else comp_statement end
     * @return
     */
    std::unique_ptr<AST::Statement> parseIfBranches(
            std::unique_ptr<AST::Statement> cond, std::unique_ptr<AST::Statement> trueBranch);

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
    std::unique_ptr<AST::CallArgs> parseCallArgs();

    /**
     * block ::= { block_body }
     *       ::= do block_body end
     * block_body ::= comp_statement
     *            ::= | func_def_args | comp_statement
     * @return
     */
    std::unique_ptr<AST::FunctionDef> parseBlock();

    /**
     * variable ::= dc_variable
     *          ::= ndc_variable
     * dc_variable ::= :: identifier
     *             ::= :: identifier dc_variable
     * ndc_variable ::= identifier
     *              ::= identifier :: ndc_variable
     * @param ident
     * @return
     */
    bool parseVariable(std::string &ident, bool greedy);

    /**
     * func_def_args ::= arg
     *               ::= arg, func_def_args
     * arg ::= identifier
     *     ::= identifier = statement
     *     ::= identifier : statement
     * @return
     */
    std::unique_ptr<AST::FuncDefArgs> parseFuncDefArgs(bool greedy);

    Lexer::TokenType nextLexerToken(bool skipSpaces = false);

    Lexer::TokenType prevLexerToken();

    Lexer::TokenType rewindTo(int ptr);

    void logError(const char *err);

    std::unique_ptr<AST::Statement> parseBinOpRight(std::unique_ptr<AST::Statement> left, int precedence = 0);

    AST::OperationType convertToBinOp(AST::OperationType type);

    AST::OperationType convertToUnOp(AST::OperationType type);

    int getOpPrecedence(AST::OperationType);

    void skipTerms();

    void skipSpaces();

    bool isATerm(Lexer::TokenType token);

    Lexer::TokenQueue *queue;
    Lexer::TokenType currentLexerToken;
};

} // UltraRuby
} // Parser

#endif //ULTRA_RUBY_PARSER_PARSER_H
