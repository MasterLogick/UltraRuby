#include <iostream>
#include <map>
#include "Lexer.h"

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
    return std::find(std::begin(opChars), std::end(opChars), c) != std::end(opChars);
}

void Lexer::emmitToken() {
    switch (lastChar) {
        case ' ':
        case '\t': {
            do {
                lastChar = input->getNextChar();
            } while (lastChar == ' ' || lastChar == '\t');
            queue.pushBack(TOK_SPACE);
            return;
        }
        case EOF:
            queue.pushBack(TOK_EOF);
            return;
        case '\n':
        case '\r': {
            //todo count new lines
            do {
                lastChar = input->getNextChar();
            } while (lastChar == '\n' || lastChar == '\r');
            queue.pushBack(TOK_NEWLINE);
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
                queue.pushBack(TOK_COMMON_STRING, std::move(str));
                return;
            }
            do {
                str += (char) lastChar;
                lastChar = input->getNextChar();
                //todo handle escape sequences
            } while (lastChar != ending);
            lastChar = input->getNextChar();
            queue.pushBack(TOK_COMMON_STRING, std::move(str));
            return;
        }

        case '#': {
            do {
                lastChar = input->getNextChar();
            } while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');
            if (lastChar == EOF) {
                queue.pushBack(TOK_EOF);
            } else {
                queue.pushBack(TOK_NEWLINE);
            }
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
                queue.pushBack(TOK_HASH_ASSOC);
                return;
            }
            if (!ops.contains(op)) {
                queue.pushBack(TOK_ERROR);
                return;
            }
            queue.pushBack(TOK_OP, ops.at(op));
            return;
        }
        case ':': {
            lastChar = input->getNextChar();
            if (lastChar == ':') {
                lastChar = input->getNextChar();
                queue.pushBack(TOK_DOUBLE_COLON);
                return;
            }
            queue.pushBack(TOK_COLON);
            return;
        }
        case ',': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_COMMA);
            return;
        }
        case '.': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_DOT);
            return;
        }
        case ';': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_SEMICOLON);
            return;
        }
        case '@': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_AT_SIGN);
            return;
        }

        case '(': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_PAREN_LEFT);
            return;
        }
        case ')': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_PAREN_RIGHT);
            return;
        }
        case '[': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_BRACKET_LEFT);
            return;
        }
        case ']': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_BRACKET_RIGHT);
            return;
        }
        case '{': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_BRACE_LEFT);
            return;
        }
        case '}': {
            lastChar = input->getNextChar();
            queue.pushBack(TOK_BRACE_RIGHT);
            return;
        }
    }
    if (isalpha(lastChar) || lastChar == '_' || lastChar == '$' || lastChar == ':') {
        std::string identifier;
        identifier += (char) lastChar;
        for (lastChar = input->getNextChar(); isalnum(lastChar) || lastChar == '_'; lastChar = input->getNextChar()) {
            identifier += (char) lastChar;
        }
        if (getKeyword(identifier) != TOK_ERROR) {
            queue.pushBack(getKeyword(identifier));
        } else {
            queue.pushBack(TOK_IDENTIFIER, std::move(identifier));
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
                logError("floating point must contain only one period");
                queue.pushBack(TOK_ERROR);
                return;
            }
            if (lastChar == '.') {
                pointIncluded = true;
            }
        } while (isdigit(lastChar) || lastChar == '.');
        if (pointIncluded) {
            queue.pushBack(TOK_FLOAT, std::move(numStr));
        } else {
            queue.pushBack(TOK_INTEGER, std::move(numStr));
        }
        return;
    }
    lastChar = input->getNextChar();
    logError("unknown input char");
    queue.pushBack(TOK_ERROR);
}

void Lexer::logError(const char *log) {
    std::cerr << "Lexer error: " << log << std::endl;
}

}