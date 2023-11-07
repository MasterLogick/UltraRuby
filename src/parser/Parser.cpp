#include <iostream>
#include <vector>
#include "Parser.h"
#include "../ast/AST.h"

namespace UltraRuby {
namespace Parser {

AST::Block *Parser::parseProgram() {
    std::vector<AST::Statement *> list;
    while (currentLexerToken != Lexer::TOK_EOF) {
        skipTerms();
        if (currentLexerToken == Lexer::TOK_EOF) {
            break;
        }
        auto v = parseTopStatement();
        if (v == nullptr) {
            break;
        }
        list.push_back(v);
    }
    return new AST::Block(std::move(list));
}

AST::Statement *Parser::parseTopStatement() {
    if (currentLexerToken == Lexer::TOK_BEGIN_UPPER) {
        nextLexerToken(true);
        if (currentLexerToken != Lexer::TOK_BRACE_LEFT) {
            logError("expected {");
            return nullptr;
        }
        nextLexerToken(true);
        auto stmt = parseProgram();
        if (currentLexerToken != Lexer::TOK_BRACE_RIGHT) {
            logError("expected }");
            return nullptr;
        }
        return stmt;
    }
    return parseStatement();
}

AST::Statement *Parser::parseStatement(int opPrecedence) {
    auto prim = parseUncontrolledStatement(opPrecedence);
    if (prim == nullptr) {
        return nullptr;
    }
    while (true) {
        switch (currentLexerToken) {
            case Lexer::TOK_IF: {
                nextLexerToken(true);
                auto cond = parseUncontrolledStatement();
                prim = new AST::If(cond, prim, nullptr);
                continue;
            }
            case Lexer::TOK_WHILE: {
                nextLexerToken(true);
                auto cond = parseUncontrolledStatement();
                prim = new AST::While(cond, prim);
                continue;
            }
            case Lexer::TOK_UNLESS: {
                nextLexerToken(true);
                auto cond = parseUncontrolledStatement();
                prim = new AST::If(new AST::UnaryOperation(AST::OperationType::UN_OP_NOT, cond), prim, nullptr);
                continue;
            }
            case Lexer::TOK_UNTIL: {
                nextLexerToken(true);
                auto cond = parseUncontrolledStatement();
                prim = new AST::While(new AST::UnaryOperation(AST::OperationType::UN_OP_NOT, cond), prim);
                continue;
            }
        }
        break;
    }
    return prim;
}

AST::Statement *Parser::parseUncontrolledStatement(int opPrecedence) {
    skipSpaces();
    if (!primaryTest()) {
        return nullptr;
    }
    auto prim = parsePrimary();
    if (prim == nullptr) {
        return nullptr;
    }
    return parseBinOpRight(prim, opPrecedence);
}

bool Parser::primaryTest() {
    switch (currentLexerToken) {
        case Lexer::TOK_COMMON_STRING:
        case Lexer::TOK_INTEGER:
        case Lexer::TOK_FLOAT:
        case Lexer::TOK_TRUE:
        case Lexer::TOK_FALSE:
        case Lexer::TOK_SELF:
        case Lexer::TOK_NIL:
        case Lexer::TOK_BEGIN:
        case Lexer::TOK_PAREN_LEFT:
        case Lexer::TOK_BRACKET_LEFT:
        case Lexer::TOK_BRACE_LEFT:
        case Lexer::TOK_RETURN:
        case Lexer::TOK_NEXT:
        case Lexer::TOK_BREAK:
        case Lexer::TOK_REDO:
        case Lexer::TOK_RETRY:
        case Lexer::TOK_YIELD:
        case Lexer::TOK_YIELD_SELF:
        case Lexer::TOK_IF:
        case Lexer::TOK_UNLESS:
        case Lexer::TOK_WHILE:
        case Lexer::TOK_UNTIL:
        case Lexer::TOK_CASE:
        case Lexer::TOK_FOR:
        case Lexer::TOK_CLASS:
        case Lexer::TOK_MODULE:
        case Lexer::TOK_DEF:
        case Lexer::TOK_IDENTIFIER:
        case Lexer::TOK_DOUBLE_COLON:
        case Lexer::TOK_COLON:
        case Lexer::TOK_OP:
            return true;
        default:
            return false;
    }
}

AST::Statement *Parser::parsePrimary() {
    auto obj = parsePrimaryObject();
    if (obj == nullptr) {
        return nullptr;
    }
    int ptr;
    while (true) {
        ptr = queue->getPtr();
        skipTerms();
        if (currentLexerToken == Lexer::TOK_DOT) {
            nextLexerToken();
            skipTerms();
            if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                logError("expected identifier");
                return nullptr;
            }
            std::string name;
            if (!parseVariable(name, false)) {
                return nullptr;
            }
            auto argsParse = parseCallArgs();
            if (argsParse.first == nullptr) {
                return nullptr;
            }
            obj = new AST::Call(std::move(name), argsParse.first, obj);
        } else if (currentLexerToken == Lexer::TOK_BRACKET_LEFT) {
            auto argsParse = parseCallArgs();
            if (argsParse.first == nullptr) {
                return nullptr;
            }
            obj = new AST::Call(std::string(), argsParse.first, obj);
        } else {
            rewindTo(ptr);
            return obj;
        }
    }
}

AST::Statement *Parser::parsePrimaryObject() {
    switch (currentLexerToken) {
        case Lexer::TOK_COMMON_STRING: {
            std::string s = queue->getStr();
            nextLexerToken();
            return new AST::String(std::move(s));
        }
        case Lexer::TOK_INTEGER: {
            std::string val = queue->getStr();
            nextLexerToken();
            return new AST::IntegerConst(std::move(val));
        }
        case Lexer::TOK_FLOAT: {
            std::string val = queue->getStr();
            nextLexerToken();
            return new AST::FloatConst(std::move(val));
        }
        case Lexer::TOK_COLON: {
            nextLexerToken();
            if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                prevLexerToken();
                return nullptr;
            }
            std::string sym = queue->getStr();
            nextLexerToken();
            return new AST::Symbol(std::move(sym));
        }
        case Lexer::TOK_TRUE: {
            nextLexerToken();
            return new AST::BoolConst(true);
        }
        case Lexer::TOK_FALSE: {
            nextLexerToken();
            return new AST::BoolConst(false);
        }
        case Lexer::TOK_SELF: {
            nextLexerToken();
            return new AST::Variable("self");
        }
        case Lexer::TOK_NIL: {
            nextLexerToken();
            return new AST::Variable("nil");
        }
        case Lexer::TOK_BEGIN: {
            nextLexerToken();
            auto mainBlock = parseCompStatement();
            if (mainBlock == nullptr) {
                return nullptr;
            }
            std::vector<std::tuple<AST::Statement *, std::string, AST::Block *>> rescues;
            while (currentLexerToken == Lexer::TOK_RESCUE) {
                nextLexerToken(true);
                AST::Statement *type(nullptr);
                std::string var;
                if (currentLexerToken == Lexer::TOK_HASH_ASSOC) {
                    nextLexerToken(true);
                    if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                        logError("expected identifier");
                        return nullptr;
                    }
                    var = queue->getStr();
                    nextLexerToken(true);
                } else if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                    type = parseStatement();
                    if (currentLexerToken == Lexer::TOK_HASH_ASSOC) {
                        nextLexerToken(true);
                        if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                            logError("expected identifier");
                            return nullptr;
                        }
                        var = queue->getStr();
                        nextLexerToken(true);
                    }
                }
                if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                    logError("expected \"then\", \";\" or \"\\n\"");
                    return nullptr;
                }
                nextLexerToken();
                auto body = parseCompStatement();
                rescues.emplace_back(type, std::move(var), body);
            }
            AST::Block *elseBlock(nullptr);
            if (currentLexerToken == Lexer::TOK_ELSE) {
                nextLexerToken();
                elseBlock = parseCompStatement();
                if (elseBlock == nullptr) {
                    return nullptr;
                }
            }
            AST::Block *ensureBlock(nullptr);
            if (currentLexerToken == Lexer::TOK_ENSURE) {
                nextLexerToken();
                ensureBlock = parseCompStatement();
                if (ensureBlock == nullptr) {
                    return nullptr;
                }
            }
            if (currentLexerToken != Lexer::TOK_END) {
                logError("expected \"end\"");
                return nullptr;
            }
            return new AST::ExceptionalBlock(mainBlock, rescues, elseBlock, ensureBlock);
        }
        case Lexer::TOK_PAREN_LEFT: {
            nextLexerToken();
            auto stmt = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_PAREN_RIGHT) {
                logError("expected \")\"");
                return nullptr;
            }
            nextLexerToken();
            return stmt;
        }
        case Lexer::TOK_BRACKET_LEFT: {
            nextLexerToken();
            return parseArray();
        }
        case Lexer::TOK_BRACE_LEFT: {
            nextLexerToken();
            return parseHash();
        }
        case Lexer::TOK_RETURN: {
            nextLexerToken();
            skipSpaces();
            return new AST::Return(parseCallArgs().first);
        }
        case Lexer::TOK_NEXT: {
            nextLexerToken();
            skipSpaces();
            if (isATerm(currentLexerToken)) {
                return new AST::Next(nullptr);
            }
            return new AST::Next(parseCallArgs().first);
        }
        case Lexer::TOK_BREAK: {
            nextLexerToken();
            skipSpaces();
            return new AST::Break(parseCallArgs().first);
        }
        case Lexer::TOK_REDO: {
            nextLexerToken();
            return new AST::Redo();
        }
        case Lexer::TOK_RETRY: {
            nextLexerToken();
            return new AST::Retry();
        }
        case Lexer::TOK_YIELD: {
            nextLexerToken();
            return new AST::Yield(parseCallArgs().first);
        }
        case Lexer::TOK_YIELD_SELF: {
            nextLexerToken();
            return new AST::YieldSelf();
        }
        case Lexer::TOK_IF: {
            nextLexerToken();
            auto cond = parseStatement();
            if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                logError("expected \"then\", \";\" or \"\\n\"");
                return nullptr;
            }
            nextLexerToken(true);
            auto body = parseCompStatement();
            if (currentLexerToken == Lexer::TOK_END) {
                nextLexerToken();
                return new AST::If(cond, body, nullptr);
            }
            return parseIfBranches(cond, body);
        }
        case Lexer::TOK_UNLESS: {
            nextLexerToken();
            auto cond = parseStatement();
            if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                logError("expected \"then\", \";\" or \"\\n\"");
                return nullptr;
            }
            nextLexerToken(true);
            auto body = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_END) {
                logError("expected \"end\"");
                return nullptr;
            }
            return new AST::If(new AST::UnaryOperation(AST::UN_OP_NOT, cond), body, nullptr);
        }
        case Lexer::TOK_WHILE: {
            nextLexerToken();
            auto cond = parseStatement();
            if (currentLexerToken != Lexer::TOK_DO && !isATerm(currentLexerToken)) {
                logError("expected \"do\", \";\" or \"\\n\"");
                return nullptr;
            }
            nextLexerToken(true);
            auto body = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_END) {
                logError("expected \"end\"");
                return nullptr;
            }
            return new AST::While(cond, body);
        }
        case Lexer::TOK_UNTIL: {
            nextLexerToken();
            auto cond = parseStatement();
            if (currentLexerToken != Lexer::TOK_DO && !isATerm(currentLexerToken)) {
                logError("expected \"do\", \";\" or \"\\n\"");
                return nullptr;
            }
            nextLexerToken(true);
            auto body = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_END) {
                logError("expected \"end\"");
                return nullptr;
            }
            return new AST::While(new AST::UnaryOperation(AST::UN_OP_NOT, cond), body);
        }
        case Lexer::TOK_CASE: {
            nextLexerToken(true);
            AST::Statement *arg(nullptr);
            if (!isATerm(currentLexerToken)) {
                arg = parseStatement();
            }
            std::vector<AST::CaseWhereBlock *> cases;
            do {
                skipTerms();
                if (currentLexerToken != Lexer::TOK_WHEN) {
                    logError("expected \"when\"");
                    return nullptr;
                }
                nextLexerToken();
                auto cond = parseStatement();
                if (cond == nullptr) {
                    return nullptr;
                }
                if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                    logError("expected \"then\", \"\\n\" or \";\"");
                    return nullptr;
                }
                nextLexerToken();
                auto body = parseCompStatement();
                if (body == nullptr) {
                    return nullptr;
                }
                cases.push_back(new AST::CaseWhereBlock(cond, body));
            } while (currentLexerToken != Lexer::TOK_END);
            nextLexerToken();
            return new AST::Case(arg, std::move(cases));
        }
        case Lexer::TOK_FOR: {
            nextLexerToken(true);
            auto var = parseStatement();
            if (var == nullptr) {
                return nullptr;
            }
            if (currentLexerToken != Lexer::TOK_IN) {
                logError("expected \"in\"");
                return nullptr;
            }
            nextLexerToken();
            auto expr = parseStatement();
            if (expr == nullptr) {
                return nullptr;
            }
            if (currentLexerToken != Lexer::TOK_DO && !isATerm(currentLexerToken)) {
                logError("expected \"do\", \"\\n\" or \";\"");
                return nullptr;
            }
            auto body = parseCompStatement();
            if (body == nullptr) {
                return nullptr;
            }
            if (currentLexerToken != Lexer::TOK_END) {
                logError("expected \"end\"");
                return nullptr;
            }
            nextLexerToken();
            return new AST::For(var, expr, body);
        }
        case Lexer::TOK_CLASS: {
            nextLexerToken(true);
            skipTerms();
            if (currentLexerToken == Lexer::TOK_OP) {
                if (queue->getOperation() != AST::BIN_OP_LEFT_SHIFT) {
                    logError("expected \"<<\"");
                    return nullptr;
                }
                nextLexerToken();
                auto instance = parseStatement();
                if (instance == nullptr) {
                    return nullptr;
                }
                auto definition = parseCompStatement();
                if (definition == nullptr) {
                    return nullptr;
                }
                if (currentLexerToken != Lexer::TOK_END) {
                    logError("expected \"end\"");
                    return nullptr;
                }
                nextLexerToken();
                return new AST::ClassInstanceDef(instance, definition);
            }
            std::string classIdentifier;
            if (!parseVariable(classIdentifier, true)) {
                return nullptr;
            }
            std::string superclassIdentifier;
            if (currentLexerToken == Lexer::TOK_OP) {
                if (queue->getOperation() != AST::BIN_OP_LESS) {
                    logError("expected \"<\"");
                    return nullptr;
                }
                nextLexerToken(true);
                if (!parseVariable(superclassIdentifier, true)) {
                    return nullptr;
                }
            }
            auto definition = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_END) {
                logError("expected \"end\"");
                return nullptr;
            }
            return new AST::ClassDef(classIdentifier, superclassIdentifier, definition);
        }
        case Lexer::TOK_MODULE: {
            nextLexerToken(true);
            std::string moduleIdentifier;
            if (!parseVariable(moduleIdentifier, true)) {
                return nullptr;
            }
            auto definition = parseCompStatement();
            if (definition == nullptr) {
                return nullptr;
            }
            if (currentLexerToken != Lexer::TOK_END) {
                logError("expected \"end\"");
                return nullptr;
            }
            nextLexerToken();
            return new AST::ModuleDef(moduleIdentifier, definition);
        }
        case Lexer::TOK_DEF: {
            nextLexerToken();
            skipTerms();
            AST::Statement *singleton(nullptr);
            std::string functionName;
            if (currentLexerToken == Lexer::TOK_IDENTIFIER) {
                //todo parse keyword variables
                std::string label = queue->getStr();
                nextLexerToken();
                skipTerms();
                if (currentLexerToken == Lexer::TOK_DOT) {
                    nextLexerToken();
                    skipTerms();
                    if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                        logError("expected identifier");
                        return nullptr;
                    }
                    functionName = queue->getStr();
                    singleton = new AST::Variable(label);
                } else {
                    functionName = label;
                }
            } else if (currentLexerToken == Lexer::TOK_PAREN_LEFT) {
                singleton = parseStatement();
                if (singleton == nullptr) {
                    return nullptr;
                }
                if (currentLexerToken != Lexer::TOK_PAREN_RIGHT) {
                    logError("expected \")\"");
                    return nullptr;
                }
                nextLexerToken();
                skipTerms();
                if (currentLexerToken != Lexer::TOK_DOT) {
                    logError("expected \".\"");
                    return nullptr;
                }
                nextLexerToken();
                skipTerms();
                if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                    logError("expected identifier");
                    return nullptr;
                }
                functionName = queue->getStr();
            } else {
                logError("expected identifier or \"(\"");
                return nullptr;
            }
            skipSpaces();
            bool hasParen = false;
            if (currentLexerToken == Lexer::TOK_PAREN_LEFT) {
                nextLexerToken();
                hasParen = true;
            }
            std::vector<AST::FuncDefArg *> args;
            if (!parseFuncDefArgs(args, hasParen)) {
                return nullptr;
            }
            if (hasParen) {
                if (currentLexerToken != Lexer::TOK_PAREN_RIGHT) {
                    logError("expected \")\"");
                    return nullptr;
                }
                nextLexerToken();
            }
            auto body = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_END) {
                logError("exepcted \"end\"");
                return nullptr;
            }
            nextLexerToken();
            return new AST::FunctionDef(functionName, args, singleton, body);
        }
        case Lexer::TOK_OP: {
            auto op = convertToUnOp(queue->getOperation());
            if (op == AST::OP_UNKNOWN) {
                return nullptr;
            }
            nextLexerToken();
            if (currentLexerToken == Lexer::TOK_SPACE) {
                prevLexerToken();
                return nullptr;
            }
            auto expr = parsePrimary();
            if (expr == nullptr) {
                return nullptr;
            }
            return new AST::UnaryOperation(op, expr);
        }
        case Lexer::TOK_NOT: {
            nextLexerToken();
            auto expr = parsePrimary();
            if (expr == nullptr) {
                return nullptr;
            }
            return new AST::UnaryOperation(AST::UN_OP_NOT, expr);
        }
        case Lexer::TOK_IDENTIFIER:
        case Lexer::TOK_DOUBLE_COLON: {
            std::string label;
            if (!parseVariable(label, false)) {
                return nullptr;
            }
            auto argsParse = parseCallArgs();
            if (argsParse.first->hasBrackets() || argsParse.second || !argsParse.first->getArgs().empty()) {
                return new AST::Call(label, argsParse.first, nullptr);
            } else {
                return new AST::Variable(label);
            }
        }
        default: {
            return nullptr;
        }
    }
}

void Parser::logError(const char *err) {
    std::cerr << "Parser error: " << err << std::endl;
}

void Parser::skipTerms() {
    while (isATerm(currentLexerToken)) {
        nextLexerToken();
    }
}

void Parser::skipSpaces() {
    // lexer squashes continuous spaces into one token
    if (currentLexerToken == Lexer::TOK_SPACE) {
        nextLexerToken();
    }
}

bool Parser::isATerm(Lexer::TokenType token) {
    return token == Lexer::TOK_NEWLINE || token == Lexer::TOK_SPACE || token == Lexer::TOK_SEMICOLON;
}

AST::Block *Parser::parseCompStatement() {
    std::vector<AST::Statement *> block;
    skipTerms();
    while (true) {
        auto stmt = parseStatement();
        if (stmt == nullptr) {
            break;
        } else {
            block.push_back(stmt);
        }
        skipTerms();
    }
    return new AST::Block(std::move(block));
}

AST::Statement *Parser::parseArray() {
    std::vector<AST::Statement *> elements;
    while (true) {
        auto el = parseStatement();
        if (el == nullptr) {
            return nullptr;
        }
        elements.push_back(el);
        if (currentLexerToken == Lexer::TOK_BRACKET_RIGHT) {
            break;
        }
        if (currentLexerToken != Lexer::TOK_COMMA) {
            logError("expected \",\"");
            return nullptr;
        }
    }
    return new AST::Array(std::move(elements));
}

AST::Statement *Parser::parseHash() {
    std::vector<std::pair<AST::Statement *, AST::Statement *>> hash;
    while (true) {
        skipTerms();
        if (currentLexerToken == Lexer::TOK_BRACE_RIGHT) {
            break;
        }
        bool symbolAssoc = false;
        if (currentLexerToken == Lexer::TOK_IDENTIFIER) {
            nextLexerToken();
            if (currentLexerToken == Lexer::TOK_SPACE) {
                nextLexerToken();
                if (currentLexerToken == Lexer::TOK_SEMICOLON) {
                    symbolAssoc = true;
                }
                prevLexerToken();
            } else if (currentLexerToken == Lexer::TOK_SEMICOLON) {
                symbolAssoc = true;
            }
            prevLexerToken();
        }
        if (symbolAssoc) {
            auto symbol = new AST::Symbol(queue->getStr());
            nextLexerToken(true);
            nextLexerToken(true);
            auto val = parseStatement();
            if (val == nullptr) {
                return nullptr;
            }
            hash.emplace_back(symbol, val);
        } else {
            auto key = parseStatement();
            if (key == nullptr) {
                return nullptr;
            }
            if (currentLexerToken != Lexer::TOK_HASH_ASSOC) {
                logError("expected \"=>\"");
                return nullptr;
            }
            nextLexerToken();
            auto val = parseStatement();
            if (val == nullptr) {
                return nullptr;
            }
            hash.emplace_back(std::move(key), std::move(val));
        }
        skipTerms();
        if (currentLexerToken == Lexer::TOK_COMMA) {
            nextLexerToken();
        } else {
            logError("expected \",\" or \"}\"");
            return nullptr;
        }
    }
    nextLexerToken();
    return new AST::Hash(std::move(hash));
}

AST::Statement *Parser::parseIfBranches(
        AST::Statement *cond, AST::Statement *trueBranch) {
    if (currentLexerToken == Lexer::TOK_ELSE) {
        nextLexerToken();
        auto body = parseCompStatement();
        if (body == nullptr) {
            return nullptr;
        }
        if (currentLexerToken != Lexer::TOK_END) {
            logError("expected \"end\"");
            return nullptr;
        }
        nextLexerToken();
        return new AST::If(cond, trueBranch, body);
    }
    if (currentLexerToken != Lexer::TOK_ELSIF) {
        logError("expected \"else\" or \"elsif\"");
        return nullptr;
    }
    nextLexerToken();
    auto cond1 = parseStatement();
    if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
        logError("expected \"then\", \";\" or \"\\n\"");
        return nullptr;
    }
    nextLexerToken(true);
    auto body = parseCompStatement();
    if (currentLexerToken == Lexer::TOK_END) {
        nextLexerToken();
        return new AST::If(cond, body, nullptr);
    }
    auto falseBranch = parseIfBranches(cond1, body);
    if (falseBranch == nullptr) {
        return nullptr;
    }
    return new AST::If(cond, trueBranch, falseBranch);
}

std::pair<AST::CallArgs *, bool> Parser::parseCallArgs() {
    bool expectTrailingParen = false;
    bool expectTrailingBracket = false;
    if (currentLexerToken == Lexer::TOK_PAREN_LEFT) {
        expectTrailingParen = true;
        nextLexerToken();
    } else {
        skipSpaces();
        if (currentLexerToken == Lexer::TOK_BRACKET_LEFT) {
            expectTrailingBracket = true;
            nextLexerToken();
        }
    }
    skipSpaces();
    if (!expectTrailingParen && !expectTrailingBracket) {
        //todo handle hash argument case case
        if (currentLexerToken == Lexer::TOK_BRACE_LEFT || currentLexerToken == Lexer::TOK_DO) {
            auto block = parseBlock();
            return {new AST::CallArgs(std::vector<AST::Statement *>(),
                                      std::map<std::string, AST::Statement *>(), block, false),
                    false};
        }
    }
    std::vector<AST::Statement *> positionalArgs;
    //todo handle named args
    std::map<std::string, AST::Statement *> namedArgs;
    while (true) {
        if (expectTrailingParen || expectTrailingBracket) {
            skipTerms();
        }
        auto stmt = parseUncontrolledStatement();
        if (stmt == nullptr) {
            if (expectTrailingParen && currentLexerToken == Lexer::TOK_PAREN_RIGHT ||
                expectTrailingBracket && currentLexerToken == Lexer::TOK_BRACKET_RIGHT) {
                nextLexerToken();
                break;
            }
            if (expectTrailingParen || expectTrailingBracket) {
                logError(expectTrailingParen ? "expected \")\"" : "expected \"]\"");
                return {nullptr, false};
            }
            break;
        }
        positionalArgs.push_back(stmt);
        if (currentLexerToken == Lexer::TOK_COMMA) {
            nextLexerToken();
        } else if (expectTrailingBracket && currentLexerToken == Lexer::TOK_BRACKET_RIGHT ||
                   expectTrailingParen && currentLexerToken == Lexer::TOK_PAREN_RIGHT) {
            nextLexerToken();
            break;
        } else {
            if (!expectTrailingParen && !expectTrailingBracket) {
                break;
            }
            if (currentLexerToken == Lexer::TOK_NEWLINE) {
                nextLexerToken();
            } else {
                logError(expectTrailingParen ? "expected \")\"" : "expected \"]\"");
                return {nullptr, false};
            }
        }
    }
    if (positionalArgs.empty() || expectTrailingParen || expectTrailingBracket) {
        skipSpaces();
        if (currentLexerToken == Lexer::TOK_BRACE_LEFT || currentLexerToken == Lexer::TOK_DO) {
            auto block = parseBlock();
            return {new AST::CallArgs(std::move(positionalArgs), std::move(namedArgs), block, expectTrailingBracket),
                    expectTrailingParen};
        }
    }
    return {new AST::CallArgs(std::move(positionalArgs), std::move(namedArgs), nullptr, expectTrailingBracket),
            expectTrailingParen};
}

AST::FunctionDef *Parser::parseBlock() {
    bool expectEndToken = currentLexerToken == Lexer::TOK_DO;
    if (currentLexerToken != Lexer::TOK_BRACE_LEFT && currentLexerToken != Lexer::TOK_DO) {
        logError("expected \"{\" or \"do\"");
        return nullptr;
    }
    nextLexerToken();
    skipTerms();
    std::vector<AST::FuncDefArg *> args;
    if (currentLexerToken == Lexer::TOK_OP && queue->getOperation() == AST::BIN_OP_BIN_OR) {
        nextLexerToken();
        if (!parseFuncDefArgs(args, true)) {
            return nullptr;
        }
        if (currentLexerToken != Lexer::TOK_OP || queue->getOperation() != AST::BIN_OP_BIN_OR) {
            logError("expected \"|\"");
            return nullptr;
        }
        nextLexerToken();
    }
    auto body = parseCompStatement();
    if (body == nullptr) {
        return nullptr;
    }
    if (expectEndToken) {
        if (currentLexerToken != Lexer::TOK_END) {
            logError("expected \"end\"");
            return nullptr;
        }
    } else {
        if (currentLexerToken != Lexer::TOK_BRACE_RIGHT) {
            logError("expected \"}\"");
            return nullptr;
        }
    }
    nextLexerToken();
    return new AST::FunctionDef(std::string(), args, nullptr, body);
}

bool Parser::parseVariable(std::string &ident, bool greedy) {
    if (greedy) {
        skipTerms();
    } else {
        skipSpaces();
    }
    bool expectIdentifierNext;
    if (currentLexerToken == Lexer::TOK_IDENTIFIER) {
        expectIdentifierNext = true;
    } else if (currentLexerToken == Lexer::TOK_DOUBLE_COLON) {
        expectIdentifierNext = false;
    } else {
        logError("expected identifier or \"::\"");
        return false;
    }
    while (true) {
        if (currentLexerToken == Lexer::TOK_IDENTIFIER) {
            if (expectIdentifierNext) {
                expectIdentifierNext = false;
                ident += queue->getStr();
                nextLexerToken();
                if (currentLexerToken == Lexer::TOK_OP) {
                    if (queue->getOperation() == AST::UN_OP_NOT) {
                        ident += '!';
                        nextLexerToken();
                    } else if (queue->getOperation() == AST::BIN_OP_QUESTION) {
                        ident += '?';
                        nextLexerToken();
                    }
                }
                if (greedy) {
                    skipTerms();
                } else {
                    skipSpaces();
                }
            } else {
                expectIdentifierNext = false;
                break;
            }
        } else if (currentLexerToken == Lexer::TOK_DOUBLE_COLON) {
            if (!expectIdentifierNext) {
                ident += "::";
                nextLexerToken(true);
                skipTerms();
                expectIdentifierNext = true;
            } else {
                break;
            }
        } else if (currentLexerToken == Lexer::TOK_SPACE || (greedy && currentLexerToken == Lexer::TOK_NEWLINE)) {
            nextLexerToken(true);
        } else {
            break;
        }
    }
    if (expectIdentifierNext) {
        logError("expected identifier");
        return false;
    }
    return true;
}

bool Parser::parseFuncDefArgs(std::vector<AST::FuncDefArg *> &args, bool greedy) {
    bool blockPresent = false;
    bool variadicPresent = false;
    bool enteredParametrizedFunctions = false;
    bool exitedParametrizedFunctions = false;
    int pos = 0;
    while (true) {
        if (greedy) {
            skipTerms();
        } else {
            skipSpaces();
        }
        if (currentLexerToken == Lexer::TOK_OP) {
            if (queue->getOperation() == AST::LEX_OP_STAR) {
                if (variadicPresent) {
                    logError("unexpected second variadic");
                    return false;
                }
                variadicPresent = true;
                if (enteredParametrizedFunctions) {
                    exitedParametrizedFunctions = true;
                }
                nextLexerToken();
                skipTerms();
                if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                    logError("expected identifier");
                    return false;
                }
                args.push_back(new AST::FuncDefArg(
                        queue->getStr(), pos, nullptr, AST::FuncDefArg::AST_ARG_TYPE_VARIADIC));
            } else if (queue->getOperation() == AST::BIN_OP_BIN_AND) {
                if (blockPresent) {
                    logError("unexpected second block");
                    return false;
                }
                blockPresent = true;
                if (enteredParametrizedFunctions) {
                    exitedParametrizedFunctions = true;
                }
                nextLexerToken();
                skipTerms();
                if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                    logError("expected identifier");
                    return false;
                }
                args.push_back(new AST::FuncDefArg(
                        queue->getStr(), pos, nullptr, AST::FuncDefArg::AST_ARG_TYPE_VARIADIC));
                nextLexerToken(true);
            } else {
                logError("unexpected token");
                return false;
            }
        } else {
            if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                break;
            }
            std::string label = queue->getStr();
            nextLexerToken();
            if (currentLexerToken == Lexer::TOK_COLON) {
                nextLexerToken(true);
                skipTerms();
                auto val = parseStatement();
                if (val == nullptr) {
                    logError("expected statement");
                    return false;
                }
                args.push_back(new AST::FuncDefArg(label, pos, val, AST::FuncDefArg::AST_ARG_TYPE_MAP));
            } else {
                skipSpaces();
                if (currentLexerToken == Lexer::TOK_OP && queue->getOperation() == AST::BIN_OP_ASSIGN) {
                    if (exitedParametrizedFunctions) {
                        logError("parameters with default values must be declared next to each other");
                        return false;
                    }
                    if (variadicPresent) {
                        logError("parameters with default values must come before variadic");
                        return false;
                    }
                    enteredParametrizedFunctions = true;
                    nextLexerToken(true);
                    skipTerms();
                    auto val = parseStatement();
                    if (val == nullptr) {
                        logError("expected statement");
                        return false;
                    }
                    args.push_back(new AST::FuncDefArg(label, pos, val, AST::FuncDefArg::AST_ARG_TYPE_NORMAL));
                } else {
                    if (enteredParametrizedFunctions) {
                        exitedParametrizedFunctions = true;
                    }
                    args.push_back(new AST::FuncDefArg(label, pos, nullptr, AST::FuncDefArg::AST_ARG_TYPE_NORMAL));
                }
            }
        }
        pos++;
        if (greedy) {
            skipTerms();
        } else {
            skipSpaces();
        }
        if (currentLexerToken != Lexer::TOK_COMMA) {
            break;
        }
        nextLexerToken();
    }
    return true;
}

Lexer::TokenType Parser::nextLexerToken(bool skipSpacesF) {
    currentLexerToken = queue->getNextToken();
    if (skipSpacesF) {
        skipSpaces();
    }
    return currentLexerToken;
}

AST::Statement *Parser::parseBinOpRight(AST::Statement *left, int precedence) {
    while (true) {
        skipSpaces();
        if (currentLexerToken != Lexer::TOK_OP) {
            return left;
        }
        int opPred = getOpPrecedence(convertToBinOp(queue->getOperation()));
        if (opPred < precedence) {
            return left;
        }
        auto operation = convertToBinOp(queue->getOperation());
        if (operation == AST::OP_UNKNOWN) {
            logError("expected binary operation");
            return nullptr;
        }
        if (operation == AST::BIN_OP_QUESTION) {
            nextLexerToken();
            auto trueBranch = parseStatement(precedence + 1);
            if (trueBranch == nullptr) {
                return nullptr;
            }
            if (currentLexerToken != Lexer::TOK_COLON) {
                logError("expected \":\"");
                return nullptr;
            }
            nextLexerToken();
            auto falseBranch = parseStatement(precedence + 1);
            if (falseBranch == nullptr) {
                return nullptr;
            }
            left = new AST::If(left, trueBranch, falseBranch);
            continue;
        }
        nextLexerToken(true);
        auto right = parsePrimary();
        if (right == nullptr) {
            return nullptr;
        }
        skipSpaces();
        int nextOpPred = 0;
        if (currentLexerToken == Lexer::TOK_OP) {
            nextOpPred = getOpPrecedence(convertToBinOp(queue->getOperation()));
        }
        if (opPred < nextOpPred) {
            right = parseBinOpRight(right, precedence + 1);
        }
        left = new AST::BinaryOperation(operation, left, right);
    }
}

const std::map<AST::OperationType, int> precedences = {
        {AST::UN_OP_LIST_UNWRAP,         29},

        {AST::UN_OP_NOT,                 27},
        {AST::UN_OP_POSITIVE,            27},
        {AST::UN_OP_BIN_NEGATION,        27},

        {AST::BIN_OP_POWER,              25},

        {AST::UN_OP_NEGATION,            23},

        {AST::BIN_OP_MULTIPLY,           21},
        {AST::BIN_OP_DIVIDE,             21},
        {AST::BIN_OP_MOD,                21},

        {AST::BIN_OP_ADD,                18},
        {AST::BIN_OP_SUBTRACT,           18},

        {AST::BIN_OP_LEFT_SHIFT,         17},
        {AST::BIN_OP_RIGHT_SHIFT,        17},

        {AST::BIN_OP_BIN_OR,             15},

        {AST::BIN_OP_BIN_AND,            13},
        {AST::BIN_OP_XOR,                13},

        {AST::BIN_OP_LESS,               11},
        {AST::BIN_OP_LESS_EQUAL,         11},
        {AST::BIN_OP_GREATER_EQUAL,      11},
        {AST::BIN_OP_GREATER,            11},

        //todo add ===, !=, =~, !~
        {AST::BIN_OP_EQUAL,              9},
        {AST::BIN_OP_CASE_EQUAL,         9},
        {AST::BIN_OP_THREE_WAY_COMPARE,  9},

        {AST::BIN_OP_AND,                7},

        {AST::BIN_OP_OR,                 5},

        {AST::BIN_OP_QUESTION,           3},

        {AST::BIN_OP_ASSIGN,             1},
        {AST::BIN_OP_BIN_OR_ASSIGN,      1},
        {AST::BIN_OP_OR_ASSIGN,          1},
        {AST::BIN_OP_BIN_AND_ASSIGN,     1},
        {AST::BIN_OP_AND_ASSIGN,         1},
        {AST::BIN_OP_XOR_ASSIGN,         1},
        {AST::BIN_OP_LEFT_SHIFT_ASSIGN,  1},
        {AST::BIN_OP_RIGHT_SHIFT_ASSIGN, 1},
        {AST::BIN_OP_ADD_ASSIGN,         1},
        {AST::BIN_OP_SUBTRACT_ASSIGN,    1},
        {AST::BIN_OP_MULTIPLY_ASSIGN,    1},
        {AST::BIN_OP_DIVIDE_ASSIGN,      1},
        {AST::BIN_OP_MOD_ASSIGN,         1},
        {AST::BIN_OP_POWER_ASSIGN,       1},

};

int Parser::getOpPrecedence(AST::OperationType type) {
    if (precedences.contains(type)) {
        return precedences.at(type);
    }
    return 0;
}

AST::OperationType Parser::convertToBinOp(AST::OperationType type) {
    if (type == AST::LEX_OP_STAR) {
        return AST::BIN_OP_MULTIPLY;
    } else if (type == AST::LEX_OP_MINUS) {
        return AST::BIN_OP_SUBTRACT;
    } else if (type == AST::LEX_OP_PLUS) {
        return AST::BIN_OP_ADD;
    } else if (type < AST::BIN_OP_REGION_START || type > AST::BIN_OP_REGION_END) {
        return AST::OP_UNKNOWN;
    } else {
        return type;
    }
}

AST::OperationType Parser::convertToUnOp(AST::OperationType type) {
    if (type == AST::LEX_OP_STAR) {
        return AST::UN_OP_LIST_UNWRAP;
    } else if (type == AST::LEX_OP_MINUS) {
        return AST::UN_OP_NEGATION;
    } else if (type == AST::LEX_OP_PLUS) {
        return AST::UN_OP_POSITIVE;
    } else if (type < AST::UN_OP_REGION_START || type > AST::UN_OP_REGION_END) {
        return AST::OP_UNKNOWN;
    } else {
        return type;
    }
}

Lexer::TokenType Parser::prevLexerToken() {
    currentLexerToken = queue->getPrevToken();
    return currentLexerToken;
}

Lexer::TokenType Parser::rewindTo(int ptr) {
    queue->setPtr(ptr - 1);
    return nextLexerToken();
}
} // UltraRuby
} // Parser

