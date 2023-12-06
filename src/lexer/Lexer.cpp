#include <iostream>
#include <map>
#include "Lexer.h"
#include "LexerException.h"

namespace UltraRuby::Lexer {
const std::map<std::string, TokenType> keywords{
        {"BEGIN",      TOK_BEGIN_UPPER},
        {"begin",      TOK_BEGIN},
        {"END",        TOK_END_UPPER},
        {"end",        TOK_END},
        {"alias",      TOK_ALIAS},
        {"and",        TOK_AND},
        {"break",      TOK_BREAK},
        {"case",       TOK_CASE},
        {"class",      TOK_CLASS},
        {"def",        TOK_DEF},
        {"defined",    TOK_DEFINED},
        {"do",         TOK_DO},
        {"else",       TOK_ELSE},
        {"elsif",      TOK_ELSIF},
        {"ensure",     TOK_ENSURE},
        {"false",      TOK_FALSE},
        {"for",        TOK_FOR},
        {"in",         TOK_IN},
        {"module",     TOK_MODULE},
        {"next",       TOK_NEXT},
        {"nil",        TOK_NIL},
        {"not",        TOK_NOT},
        {"or",         TOK_OR},
        {"redo",       TOK_REDO},
        {"rescue",     TOK_RESCUE},
        {"retry",      TOK_RETRY},
        {"return",     TOK_RETURN},
        {"self",       TOK_SELF},
        {"super",      TOK_SUPER},
        {"then",       TOK_THEN},
        {"true",       TOK_TRUE},
        {"undef",      TOK_UNDEF},
        {"when",       TOK_WHEN},
        {"yield",      TOK_YIELD},
        {"yield_self", TOK_YIELD_SELF},
        {"if",         TOK_IF},
        {"unless",     TOK_UNLESS},
        {"while",      TOK_WHILE},
        {"until",      TOK_UNTIL},
};

const char opChars[] = {'|', '&', '^', '+', '-', '*', '/', '%', '~', '>', '<', '!', '=', '?'};
const std::map<std::string, AST::OperationType> ops = {
        {"<",   AST::BIN_OP_LESS},
        {"<=",  AST::BIN_OP_LESS_EQUAL},
        {"==",  AST::BIN_OP_EQUAL},
        {"===", AST::BIN_OP_CASE_EQUAL},
        {">=",  AST::BIN_OP_GREATER_EQUAL},
        {">",   AST::BIN_OP_GREATER},
        {"<=>", AST::BIN_OP_THREE_WAY_COMPARE},
        {"|",   AST::BIN_OP_BIN_OR},
        {"||",  AST::BIN_OP_OR},
        {"&",   AST::BIN_OP_BIN_AND},
        {"&&",  AST::BIN_OP_AND},
        {"^",   AST::BIN_OP_XOR},
        {"<<",  AST::BIN_OP_LEFT_SHIFT},
        {">>",  AST::BIN_OP_RIGHT_SHIFT},
        {"+",   AST::LEX_OP_PLUS},
        {"-",   AST::LEX_OP_MINUS},
        {"*",   AST::LEX_OP_STAR},
        {"/",   AST::BIN_OP_DIVIDE},
        {"%",   AST::BIN_OP_MOD},
        {"**",  AST::BIN_OP_POWER},
        {"=",   AST::BIN_OP_ASSIGN},
        {"|=",  AST::BIN_OP_BIN_OR_ASSIGN},
        {"||=", AST::BIN_OP_OR_ASSIGN},
        {"&=",  AST::BIN_OP_BIN_AND_ASSIGN},
        {"&&=", AST::BIN_OP_AND_ASSIGN},
        {"^=",  AST::BIN_OP_XOR_ASSIGN},
        {"<<=", AST::BIN_OP_LEFT_SHIFT_ASSIGN},
        {">>=", AST::BIN_OP_RIGHT_SHIFT_ASSIGN},
        {"+=",  AST::BIN_OP_ADD_ASSIGN},
        {"-=",  AST::BIN_OP_SUBTRACT_ASSIGN},
        {"*=",  AST::BIN_OP_MULTIPLY_ASSIGN},
        {"/=",  AST::BIN_OP_DIVIDE_ASSIGN},
        {"%=",  AST::BIN_OP_MOD_ASSIGN},
        {"**=", AST::BIN_OP_POWER_ASSIGN},
        {"!",   AST::UN_OP_NOT},
        {"~",   AST::UN_OP_BIN_NEGATION},
        {"?",   AST::BIN_OP_QUESTION},
};

TokenType getKeyword(const std::string &identifier) {
    if (keywords.contains(identifier)) {
        return keywords.at(identifier);
    }
    return TOK_ERROR;
}

bool isOp(int c) {
    for (const auto &item: opChars) {
        if (item == c) {
            return true;
        }
    }
    return false;
}

Lexer::Lexer(std::shared_ptr<StringLexerInput> input) :
        input(std::move(input)), lastChar(' '), queue(this), eofReached(false), row(1) {
    lineBeginnings.push_back(0);
    emmitToken();
    queue.removeLast();
}

void Lexer::emmitToken() {
    int lineStart = lineBeginnings.back();
    int col = input->getPos() - lineStart;
    if (input->eof()) {
        if (!eofReached) {
            queue.pushBack(TOK_EOF, row, col);
        }
        return;
    }
    switch (lastChar) {
        case ' ':
        case '\t': {
            do {
                lastChar = input->getNextChar();
            } while (lastChar == ' ' || lastChar == '\t');
            queue.pushBack(TOK_SPACES, row, col);
            return;
        }
        case '\n':
        case '\r': {
            int oldRow = row;
            do {
                if (lastChar == '\r') {
                    lastChar = input->getNextChar();
                    if (lastChar == '\n') {
                        lastChar = input->getNextChar();
                    }
                } else {
                    lastChar = input->getNextChar();
                }
                lineBeginnings.push_back(input->getPos());
                row++;
            } while (lastChar == '\n' || lastChar == '\r');
            queue.pushBack(TOK_NEWLINE, row, oldRow);
            return;
        }

        case '`':
        case '"':
        case '\'': {
            int ending = lastChar;
            lastChar = input->getNextChar();
            std::string str;
            if (lastChar == '"') {
                lastChar = input->getNextChar();
                queue.pushBack(TOK_COMMON_STRING, std::move(str), row, col);
                return;
            }
            do {
                str += (char) lastChar;
                lastChar = input->getNextChar();
                //todo handle escape sequences
            } while (lastChar != ending);
            lastChar = input->getNextChar();
            queue.pushBack(TOK_COMMON_STRING, std::move(str), row, col);
            return;
        }

        case '#': {
            do {
                lastChar = input->getNextChar();
            } while (!input->eof() && lastChar != '\n' && lastChar != '\r');
            emmitToken();
            return;
        }

        case '|':
        case '&':
        case '^':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '~':
        case '>':
        case '<':
        case '!':
        case '=':
        case '?': {
            // todo handle "2 +- 2 == 0" cases
            std::string op;
            do {
                op += (char) lastChar;
                lastChar = input->getNextChar();
            } while (isOp(lastChar));
            if (op == "=>") {
                queue.pushBack(TOK_HASH_ASSOC, row, col);
                return;
            }
            if (!ops.contains(op)) {
                queue.pushBack(TOK_ERROR, row, col);
                return;
            }
            queue.pushBack(TOK_OP, ops.at(op), row, col);
            return;
        }
        case ':': {
            lastChar = input->getNextChar();
            if (lastChar == ':') {
                lastChar = input->getNextChar();
                queue.pushBack(TOK_DOUBLE_COLON, row, col);
                return;
            }
            queue.pushBack(TOK_COLON, row, col);
            return;
        }
        case ',': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_COMMA, row, col);
            return;
        }
        case '.': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_DOT, row, col);
            return;
        }
        case ';': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_SEMICOLON, row, col);
            return;
        }
        case '@': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_AT_SIGN, row, col);
            return;
        }

        case '(': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_PAREN_LEFT, row, col);
            return;
        }
        case ')': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_PAREN_RIGHT, row, col);
            return;
        }
        case '[': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_BRACKET_LEFT, row, col);
            return;
        }
        case ']': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_BRACKET_RIGHT, row, col);
            return;
        }
        case '{': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_BRACE_LEFT, row, col);
            return;
        }
        case '}': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_BRACE_RIGHT, row, col);
            return;
        }
    }
    if (isalpha(lastChar) || lastChar == '_' || lastChar == '$' || lastChar == ':') {
        std::string identifier;
        identifier += (char) lastChar;
        for (lastChar = input->getNextChar();
             isalnum(lastChar) || lastChar == '_'; lastChar = input->getNextChar()) {
            identifier += (char) lastChar;
        }
        if (getKeyword(identifier) != TOK_ERROR) {
            queue.pushBack(getKeyword(identifier), row, col);
        } else {
            queue.pushBack(TOK_IDENTIFIER, std::move(identifier), row, col);
        }
        return;
    }
    if (isdigit(lastChar)) {
        bool pointIncluded = false;
        std::string numStr;
        do {
            numStr += (char) lastChar;
            lastChar = input->getNextChar();
            if (pointIncluded && lastChar == '.') {
                throw LexerException("floating point must contain only one period", row, col);
            }
            if (lastChar == '.') {
                pointIncluded = true;
            }
        } while (isdigit(lastChar) || lastChar == '.');
        if (pointIncluded) {
            queue.pushBack(TOK_FLOAT, std::move(numStr), row, col);
        } else {
            queue.pushBack(TOK_INTEGER, std::move(numStr), row, col);
        }
        return;
    }
    lastChar = input->getNextChar();
    throw LexerException("unknown input char", row, col);
}

}