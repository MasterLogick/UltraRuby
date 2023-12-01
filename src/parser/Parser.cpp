#include <vector>
#include "Parser.h"
#include "../ast/AST.h"
#include "ParseRewindGuard.h"
#include "ParseException.h"

namespace UltraRuby {
namespace Parser {

//todo cleanup resources on failure


AST::Block *Parser::parseProgram() {
    std::vector<AST::Statement *> list;
    while (currentLexerToken != Lexer::TOK_EOF) {
        skipTerms();
        if (currentLexerToken == Lexer::TOK_EOF) {
            break;
        }
        list.push_back(parseTopStatement());
    }
    if (currentLexerToken != Lexer::TOK_EOF) {
        throw ParseException("left unparsed tokens");
    }
    return new AST::Block(std::move(list));
}

AST::Statement *Parser::parseTopStatement() {
    if (currentLexerToken == Lexer::TOK_BEGIN_UPPER) {
        nextLexerToken(true);
        if (currentLexerToken != Lexer::TOK_BRACE_LEFT) {
            throw ParseException("expected {");
        }
        nextLexerToken(true);
        auto stmt = parseProgram();
        if (currentLexerToken != Lexer::TOK_BRACE_RIGHT) {
            throw ParseException("expected }");
        }
        return stmt;
    }
    return parseStatement();
}

AST::Statement *Parser::parseStatement(int opPrecedence) {
    auto prim = parseUncontrolledStatement(opPrecedence);
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
            default: {
                return prim;
            }
        }
    }
}

AST::Statement *Parser::parseUncontrolledStatement(int opPrecedence) {
    skipSpaces();
    if (!primaryTest()) {
        throw ParseException("not a primary statement");
    }
    auto prim = parsePrimary();
    auto res = parseBinOpRight(prim, opPrecedence);
    return res;
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
        case Lexer::TOK_AT_SIGN:
            return true;
        default:
            return false;
    }
}

AST::Statement *Parser::parsePrimary() {
    auto obj = parsePrimaryObject();
    int ptr;
    while (true) {
        ptr = queue->getPtr();
        skipSpaces();
        if (currentLexerToken == Lexer::TOK_DOT) {
            nextLexerToken();
            skipTerms();
            if (currentLexerToken == Lexer::TOK_PAREN_LEFT) {
                auto args = parseCallArgs();
                obj = new AST::Call("call", args, obj);
            } else {
                if (!testName(false)) {
                    throw ParseException("expected identifier");
                }
                std::string name = parseName(false);
                auto args = parseCallArgs();
                if (args->hasBrackets()) {
                    obj = new AST::Call("", args,
                                        new AST::Call(std::move(name), new AST::CallArgs({}, {}, nullptr, false,
                                                                                         true), obj));
                } else {
                    obj = new AST::Call(std::move(name), args, obj);
                }
            }
        } else if (currentLexerToken == Lexer::TOK_BRACKET_LEFT) {
            auto args = parseCallArgs();
            obj = new AST::Call("", args, obj);
        } else if (currentLexerToken == Lexer::TOK_DOUBLE_COLON) {
            nextLexerToken();
            if (!testName(false)) {
                throw ParseException("expected name");
            }
            obj = new AST::ConstantRef(obj, parseName(false));
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
                throw ParseException("expected identifier");
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
            return new AST::LangVariable(AST::LangVariable::LVT_SELF);
        }
        case Lexer::TOK_NIL: {
            nextLexerToken();
            return new AST::LangVariable(AST::LangVariable::LVT_NIL);
        }
        case Lexer::TOK_BEGIN: {
            nextLexerToken();
            auto mainBlock = parseCompStatement();
            std::vector<AST::Rescue *> rescues;
            while (currentLexerToken == Lexer::TOK_RESCUE) {
                nextLexerToken(true);
                AST::Statement *type(nullptr);
                std::string var;
                if (currentLexerToken == Lexer::TOK_HASH_ASSOC) {
                    nextLexerToken(true);
                    if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                        throw ParseException("expected identifier");
                    }
                    var = queue->getStr();
                    nextLexerToken(true);
                } else if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                    type = parseStatement();
                    if (currentLexerToken == Lexer::TOK_HASH_ASSOC) {
                        nextLexerToken(true);
                        if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                            throw ParseException("expected identifier");
                        }
                        var = queue->getStr();
                        nextLexerToken(true);
                    }
                }
                if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                    throw ParseException("expected \"then\", \";\" or \"\\n\"");
                }
                nextLexerToken();
                auto body = parseCompStatement();
                rescues.push_back(new AST::Rescue(type, std::move(var), body));
            }
            if (currentLexerToken == Lexer::TOK_ELSE) {
                nextLexerToken();
                auto elseBlock = parseCompStatement();
                mainBlock = new AST::Block({mainBlock, elseBlock});
            }
            AST::Block *ensureBlock(nullptr);
            if (currentLexerToken == Lexer::TOK_ENSURE) {
                nextLexerToken();
                ensureBlock = parseCompStatement();
            }
            if (currentLexerToken != Lexer::TOK_END) {
                throw ParseException("expected \"end\"");
            }
            if (rescues.empty() && ensureBlock == nullptr) {
                return mainBlock;
            }
            return new AST::ExceptionalBlock(mainBlock, std::move(rescues), ensureBlock);
        }
        case Lexer::TOK_PAREN_LEFT: {
            nextLexerToken();
            auto stmt = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_PAREN_RIGHT) {
                throw ParseException("expected \")\"");
            }
            nextLexerToken();
            return stmt;
        }
        case Lexer::TOK_BRACKET_LEFT: {
            nextLexerToken();
            auto arr = parseArray();
            return arr;
        }
        case Lexer::TOK_BRACE_LEFT: {
            nextLexerToken();
            auto hash = parseHash();
            return hash;
        }
        case Lexer::TOK_RETURN: {
            nextLexerToken();
            skipSpaces();
            auto callArgs = parseCallArgs();
            return new AST::Return(callArgs);
        }
        case Lexer::TOK_NEXT: {
            nextLexerToken();
            skipSpaces();
            auto callArgs = parseCallArgs();
            return new AST::Next(callArgs);
        }
        case Lexer::TOK_BREAK: {
            nextLexerToken();
            skipSpaces();
            auto callArgs = parseCallArgs();
            return new AST::Break(callArgs);
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
            auto callArgs = parseCallArgs();
            return new AST::Yield(callArgs);
        }
        case Lexer::TOK_YIELD_SELF: {
            nextLexerToken();
            return new AST::YieldSelf();
        }
        case Lexer::TOK_IF: {
            nextLexerToken();
            auto cond = parseStatement();
            if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                throw ParseException("expected \"then\", \";\" or \"\\n\"");
            }
            nextLexerToken(true);
            auto body = parseCompStatement();
            if (currentLexerToken == Lexer::TOK_END) {
                nextLexerToken();
                return new AST::If(cond, body, nullptr);
            }
            auto parseRes = parseIfBranches(cond, body);
            return parseRes;
        }
        case Lexer::TOK_UNLESS: {
            nextLexerToken();
            auto cond = parseStatement();
            if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                throw ParseException("expected \"then\", \";\" or \"\\n\"");
            }
            nextLexerToken(true);
            auto body = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_END) {
                throw ParseException("expected \"end\"");
            }
            return new AST::If(new AST::UnaryOperation(AST::UN_OP_NOT, cond), body, nullptr);
        }
        case Lexer::TOK_WHILE: {
            nextLexerToken();
            auto cond = parseStatement();
            if (currentLexerToken != Lexer::TOK_DO && !isATerm(currentLexerToken)) {
                throw ParseException("expected \"do\", \";\" or \"\\n\"");
            }
            nextLexerToken(true);
            auto body = parseCompStatement();
            return new AST::While(cond, body);
        }
        case Lexer::TOK_UNTIL: {
            nextLexerToken();
            auto cond = parseStatement();
            if (currentLexerToken != Lexer::TOK_DO && !isATerm(currentLexerToken)) {
                throw ParseException("expected \"do\", \";\" or \"\\n\"");
            }
            nextLexerToken(true);
            auto body = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_END) {
                throw ParseException("expected \"end\"");
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
                    throw ParseException("expected \"when\"");
                }
                nextLexerToken();
                auto cond = parseStatement();
                if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
                    throw ParseException("expected \"then\", \"\\n\" or \";\"");
                }
                nextLexerToken();
                auto body = parseCompStatement();
                cases.push_back(new AST::CaseWhereBlock(cond, body));
            } while (currentLexerToken != Lexer::TOK_END);
            nextLexerToken();
            return new AST::Case(arg, std::move(cases));
        }
        case Lexer::TOK_FOR: {
            nextLexerToken(true);
            auto var = parseStatement();
            if (currentLexerToken != Lexer::TOK_IN) {
                throw ParseException("expected \"in\"");
            }
            nextLexerToken();
            auto expr = parseStatement();
            if (currentLexerToken != Lexer::TOK_DO && !isATerm(currentLexerToken)) {
                throw ParseException("expected \"do\", \"\\n\" or \";\"");
            }
            auto body = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_END) {
                throw ParseException("expected \"end\"");
            }
            nextLexerToken();
            return new AST::For(var, expr, body);
        }
        case Lexer::TOK_CLASS: {
            nextLexerToken(true);
            skipTerms();
            if (currentLexerToken == Lexer::TOK_OP) {
                if (queue->getOperation() != AST::BIN_OP_LEFT_SHIFT) {
                    throw ParseException("expected \"<<\"");
                }
                nextLexerToken();
                auto instance = parseStatement();
                auto definition = parseCompStatement();
                if (currentLexerToken != Lexer::TOK_END) {
                    throw ParseException("expected \"end\"");
                }
                nextLexerToken();
                return new AST::ClassInstanceDef(instance, definition);
            }
            auto *name = parsePrimary();
            AST::Statement *superclass;
            if (currentLexerToken == Lexer::TOK_OP) {
                if (queue->getOperation() != AST::BIN_OP_LESS) {
                    throw ParseException("expected \"<\" or class definition");
                }
                nextLexerToken(true);
                superclass = parseStatement();
            } else {
                superclass = new AST::ConstantRef(nullptr, "Object");
            }
            auto definition = parseCompStatement();
            skipTerms();
            if (currentLexerToken != Lexer::TOK_END) {
                throw ParseException("expected \"end\"");
            }
            nextLexerToken();
            return new AST::ClassDef(name, superclass, definition);
        }
        case Lexer::TOK_MODULE: {
            nextLexerToken(true);
            auto *name = parsePrimary();
            auto definition = parseCompStatement();
            if (currentLexerToken != Lexer::TOK_END) {
                throw ParseException("expected \"end\"");
            }
            nextLexerToken();
            return new AST::ModuleDef(name, definition);
        }
        case Lexer::TOK_DEF:
            return parseFunctionDef();
        case Lexer::TOK_OP: {
            //todo handle degenerate cases of XXX#! and XXX#? method calls
            auto op = convertToUnOp(queue->getOperation());
            nextLexerToken();
            auto expr = parsePrimary();
            return new AST::UnaryOperation(op, expr);
        }
        case Lexer::TOK_NOT: {
            nextLexerToken();
            auto expr = parsePrimary();
            return new AST::UnaryOperation(AST::UN_OP_NOT, expr);
        }
        case Lexer::TOK_AT_SIGN: {
            nextLexerToken();
            if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                throw ParseException("expected identifier");
            }
            auto *var = new AST::InstanceVariable("@" + queue->getStr());
            nextLexerToken();
            return var;
        }
        case Lexer::TOK_IDENTIFIER: {
            auto name = parseName(false);
            if ('A' <= name.front() && name.front() <= 'Z') {
                try {
                    ParseRewindGuard prg(this);
                    auto *callArgs = parseCallArgs();
                    prg.release();
                    if (callArgs->getArgs().empty()) {
                        delete callArgs;
                        return new AST::ConstantRef(nullptr, std::move(name));
                    } else {
                        return new AST::Call(std::move(name), callArgs, nullptr);
                    }
                } catch (ParseException &e) {
                    return new AST::ConstantRef(nullptr, std::move(name));
                }
            }
            try {
                ParseRewindGuard prg(this);
                auto *callArgs = parseCallArgs();
                prg.release();
                return new AST::Call(name, callArgs, nullptr);
            } catch (ParseException &e) {
                return new AST::LocalVariable(std::move(name));
            }
        }
        case Lexer::TOK_DOUBLE_COLON: {
            nextLexerToken();
            skipTerms();
            if (!testName(false)) {
                throw ParseException("expected constant/method name");
            }
            std::string name = parseName(false);
            return new AST::ConstantRef(nullptr, name);
        }
        default:
            return nullptr;
    }
}

void Parser::skipTerms() {
    while (isATerm(currentLexerToken)) {
        nextLexerToken();
    }
}

void Parser::skipSpaces() {
    // lexer squashes continuous spaces into one token
    if (currentLexerToken == Lexer::TOK_SPACES) {
        nextLexerToken();
    }
}

bool Parser::isATerm(Lexer::TokenType token) {
    return token == Lexer::TOK_NEWLINE || token == Lexer::TOK_SPACES || token == Lexer::TOK_SEMICOLON;
}

AST::Block *Parser::parseCompStatement() {
    std::vector<AST::Statement *> block;
    skipTerms();
    while (true) {
        try {
            ParseRewindGuard prg(this);
            auto stmt = parseStatement();
            prg.release();
            block.push_back(stmt);
        } catch (ParseException &exception) {
            break;
        }
        skipTerms();
    }
    return new AST::Block(std::move(block));
}

AST::Statement *Parser::parseArray() {
    std::vector<AST::Statement *> elements;
    skipTerms();
    if (currentLexerToken == Lexer::TOK_BRACKET_RIGHT) {
        nextLexerToken();
        return new AST::Array(std::move(elements));
    }
    while (true) {
        auto el = parseStatement();
        elements.push_back(el);
        if (currentLexerToken == Lexer::TOK_COMMA) {
            nextLexerToken();
        } else if (currentLexerToken == Lexer::TOK_BRACKET_RIGHT) {
            nextLexerToken();
            return new AST::Array(std::move(elements));
        } else {
            throw ParseException("expected \",\"");
        }
    }
}

AST::Statement *Parser::parseHash() {
    std::vector<std::pair<AST::Statement *, AST::Statement *>> hash;
    skipTerms();
    if (currentLexerToken == Lexer::TOK_BRACE_RIGHT) {
        nextLexerToken();
        return new AST::Hash(std::move(hash));
    }
    while (true) {
        bool symbolAssoc = false;
        int ptr = queue->getPtr();
        if (currentLexerToken == Lexer::TOK_IDENTIFIER) {
            nextLexerToken(true);
            if (currentLexerToken == Lexer::TOK_COLON) {
                symbolAssoc = true;
            }
            rewindTo(ptr);
        }
        if (symbolAssoc) {
            auto symbol = new AST::Symbol(queue->getStr());
            //skip symbol name
            nextLexerToken(true);
            // skip column
            nextLexerToken(true);
            auto val = parseStatement();
            hash.emplace_back(symbol, val);
        } else {
            auto key = parseStatement();
            if (currentLexerToken != Lexer::TOK_HASH_ASSOC) {
                throw ParseException("expected \"=>\"");
            }
            nextLexerToken();
            auto val = parseStatement();
            hash.emplace_back(key, val);
        }
        skipTerms();
        if (currentLexerToken == Lexer::TOK_COMMA) {
            nextLexerToken();
        } else if (currentLexerToken == Lexer::TOK_BRACE_RIGHT) {
            nextLexerToken();
            return new AST::Hash(std::move(hash));
        } else {
            throw ParseException("expected \",\" or \"}\"");
        }
    }
}

AST::Statement *Parser::parseIfBranches(
        AST::Statement *cond, AST::Statement *trueBranch) {
    if (currentLexerToken == Lexer::TOK_ELSE) {
        nextLexerToken();
        auto body = parseCompStatement();
        if (currentLexerToken != Lexer::TOK_END) {
            throw ParseException("expected \"end\"");
        }
        nextLexerToken();
        return new AST::If(cond, trueBranch, body);
    }
    if (currentLexerToken != Lexer::TOK_ELSIF) {
        throw ParseException("expected \"else\" or \"elsif\"");
    }
    nextLexerToken();
    auto cond1 = parseStatement();
    if (currentLexerToken != Lexer::TOK_THEN && !isATerm(currentLexerToken)) {
        throw ParseException("expected \"then\", \";\" or \"\\n\"");
    }
    nextLexerToken(true);
    auto body = parseCompStatement();
    if (currentLexerToken == Lexer::TOK_END) {
        nextLexerToken();
        return new AST::If(cond, body, nullptr);
    }
    auto falseBranch = parseIfBranches(cond1, body);
    return new AST::If(cond, trueBranch, falseBranch);
}

AST::CallArgs *Parser::parseCallArgs() {
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
        if (currentLexerToken == Lexer::TOK_BRACE_LEFT || currentLexerToken == Lexer::TOK_DO) {
            auto block = parseLambdaBlock();
            return new AST::CallArgs(std::vector<AST::Statement *>(), std::map<std::string, AST::Statement *>(), block,
                                     false, false);
        }
    }
    std::vector<AST::Statement *> positionalArgs;
    //todo handle named args
    std::map<std::string, AST::Statement *> namedArgs;
    while (true) {
        if (expectTrailingParen || expectTrailingBracket) {
            skipTerms();
        }
        AST::Statement *stmt;
        try {
            ParseRewindGuard prg(this);
            stmt = parseUncontrolledStatement();
            prg.release();
        } catch (ParseException &) {
            if (expectTrailingParen && currentLexerToken == Lexer::TOK_PAREN_RIGHT ||
                expectTrailingBracket && currentLexerToken == Lexer::TOK_BRACKET_RIGHT) {
                nextLexerToken();
                break;
            }
            if (expectTrailingParen || expectTrailingBracket) {
                throw ParseException(expectTrailingParen ? "expected \")\"" : "expected \"]\"");
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
                throw ParseException(expectTrailingParen ? "expected \")\"" : "expected \"]\"");
            }
        }
    }
    if (positionalArgs.empty() || expectTrailingParen || expectTrailingBracket) {
        skipSpaces();
        if (currentLexerToken == Lexer::TOK_BRACE_LEFT || currentLexerToken == Lexer::TOK_DO) {
            auto block = parseLambdaBlock();
            return new AST::CallArgs(std::move(positionalArgs), std::move(namedArgs), block, expectTrailingBracket,
                                     expectTrailingParen);
        }
    }
    return new AST::CallArgs(std::move(positionalArgs), std::move(namedArgs), nullptr, expectTrailingBracket,
                             expectTrailingParen);
}

AST::FunctionDef *Parser::parseLambdaBlock() {
    bool expectEndToken = currentLexerToken == Lexer::TOK_DO;
    if (currentLexerToken != Lexer::TOK_BRACE_LEFT && currentLexerToken != Lexer::TOK_DO) {
        throw ParseException("expected \"{\" or \"do\"");
    }
    nextLexerToken();
    skipTerms();
    std::tuple<std::vector<std::string>, std::vector<AST::OptionalArg *>, std::string,
            std::vector<std::string>, std::vector<AST::OptionalArg *>, std::string> args;
    if (currentLexerToken == Lexer::TOK_OP && queue->getOperation() == AST::BIN_OP_BIN_OR) {
        nextLexerToken();
        args = parseFuncDefArgs(true);
        if (currentLexerToken != Lexer::TOK_OP || queue->getOperation() != AST::BIN_OP_BIN_OR) {
            throw ParseException("expected \"|\"");
        }
        nextLexerToken();
    }
    auto body = parseCompStatement();
    if (expectEndToken) {
        if (currentLexerToken != Lexer::TOK_END) {
            throw ParseException("expected \"end\"");
        }
    } else {
        if (currentLexerToken != Lexer::TOK_BRACE_RIGHT) {
            throw ParseException("expected \"}\"");
        }
    }
    nextLexerToken();
    return new AST::FunctionDef("", body,
                                std::get<0>(args),
                                std::get<1>(args),
                                std::get<2>(args),
                                std::get<3>(args),
                                std::get<4>(args),
                                std::get<5>(args),
                                nullptr);
}

std::tuple<std::vector<std::string>, std::vector<AST::OptionalArg *>, std::string,
        std::vector<std::string>, std::vector<AST::OptionalArg *>, std::string> Parser::parseFuncDefArgs(bool greedy) {
    std::vector<std::string> requiredArgsPrefix;
    std::vector<AST::OptionalArg *> optionalArgs;
    std::string variadicArg;
    std::vector<std::string> requiredArgsSuffix;
    std::vector<AST::OptionalArg *> namedArgs;
    std::string blockArg;

    // 0 - required prefix
    // 1 - optional
    // 2 - variadic
    // 3 - required suffix
    // 4 - named args
    // todo handle `**args` arg type
    int stage = 0;
    while (true) {
        if (greedy) {
            skipTerms();
        } else {
            skipSpaces();
        }
        if (currentLexerToken == Lexer::TOK_OP) {
            if (queue->getOperation() == AST::LEX_OP_STAR && stage < 2) {
                if (!variadicArg.empty()) {
                    throw ParseException("unexpected second variadic");
                }
                stage = 3;
                nextLexerToken();
                skipTerms();
                if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                    throw ParseException("expected identifier");
                }
                variadicArg = queue->getStr();
                nextLexerToken(true);
            } else if (queue->getOperation() == AST::BIN_OP_BIN_AND) {
                if (!blockArg.empty()) {
                    throw ParseException("unexpected second block");
                }
                nextLexerToken();
                skipTerms();
                if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                    throw ParseException("expected identifier");
                }
                blockArg = queue->getStr();
                nextLexerToken(true);
            } else {
                throw ParseException("unexpected token");
            }
        } else {
            if (currentLexerToken != Lexer::TOK_IDENTIFIER) {
                break;
            }
            std::string label = queue->getStr();
            nextLexerToken();
            if (currentLexerToken == Lexer::TOK_COLON) {
                stage = 4;
                nextLexerToken(true);
                skipTerms();
                try {
                    ParseRewindGuard prg(this);
                    auto val = parseStatement();
                    prg.release();
                    namedArgs.push_back(new AST::OptionalArg(std::move(label), val));
                } catch (ParseException &e) {
                    namedArgs.push_back(new AST::OptionalArg(std::move(label), nullptr));
                }
            } else {
                skipSpaces();
                if (currentLexerToken == Lexer::TOK_OP && queue->getOperation() == AST::BIN_OP_ASSIGN) {
                    if (stage > 1) {
                        throw ParseException("parameters with default values must come before variadic");
                    }
                    stage = 1;
                    nextLexerToken(true);
                    skipTerms();
                    auto val = parseStatement();
                    optionalArgs.push_back(new AST::OptionalArg(std::move(label), val));
                } else {
                    if (!optionalArgs.empty() || !variadicArg.empty() || !namedArgs.empty()) {
                        if (stage > 3) {
                            throw ParseException("required parameters come before named args");
                        } else {
                            stage = 3;
                        }
                        requiredArgsSuffix.push_back(std::move(label));
                    } else {
                        requiredArgsPrefix.push_back(std::move(label));
                    }
                }
            }
        }
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
    return std::make_tuple(std::move(requiredArgsPrefix),
                           std::move(optionalArgs),
                           std::move(variadicArg),
                           std::move(requiredArgsSuffix),
                           std::move(namedArgs),
                           std::move(blockArg));
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
            throw ParseException("expected binary operation");
        }
        if (operation == AST::BIN_OP_QUESTION) {
            nextLexerToken();
            auto trueBranch = parseStatement(precedence + 1);
            if (currentLexerToken != Lexer::TOK_COLON) {
                throw ParseException("expected \":\"");
            }
            nextLexerToken();
            auto falseBranch = parseStatement(precedence + 1);
            left = new AST::If(left, trueBranch, falseBranch);
            continue;
        }
        nextLexerToken(true);
        auto right = parsePrimary();
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

        {AST::UN_OP_NEGATIVE,            23},

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
    throw ParseException("unknown operation");
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
        return AST::UN_OP_NEGATIVE;
    } else if (type == AST::LEX_OP_PLUS) {
        return AST::UN_OP_POSITIVE;
    } else if (type < AST::UN_OP_REGION_START || type > AST::UN_OP_REGION_END) {
        throw ParseException("unknown operation");
    } else {
        return type;
    }
}

Lexer::TokenType Parser::rewindTo(int ptr) {
    queue->setPtr(ptr - 1);
    return nextLexerToken();
}

std::string Parser::parseFunctionName() {
    std::string name;
    if (testName(true)) {
        return parseName(true);
    }
    if (currentLexerToken == Lexer::TOK_AT_SIGN) {
        nextLexerToken();
        if (currentLexerToken != Lexer::TOK_OP) {
            throw ParseException("expected \"+\", or \"-\"");
        }
        switch (queue->getOperation()) {
            case AST::LEX_OP_PLUS: {
                nextLexerToken();
                return "@+";
            }
            case AST::LEX_OP_MINUS: {
                nextLexerToken();
                return "@-";
            }
            default:
                throw ParseException("expected \"+\", or \"-\"");
        }
    }
    if (currentLexerToken == Lexer::TOK_OP) {
        auto op = queue->getOperation();
        nextLexerToken();
        switch (op) {
            case AST::BIN_OP_LESS:
                return "<";
            case AST::BIN_OP_LESS_EQUAL:
                return "<=";
            case AST::BIN_OP_EQUAL:
                return "==";
            case AST::BIN_OP_CASE_EQUAL:
                return "===";
            case AST::BIN_OP_GREATER_EQUAL:
                return ">=";
            case AST::BIN_OP_GREATER:
                return ">";
            case AST::BIN_OP_THREE_WAY_COMPARE:
                return "<=>";
            case AST::BIN_OP_BIN_OR:
                return "|";
            case AST::BIN_OP_OR:
                return "||";
            case AST::BIN_OP_BIN_AND:
                return "&";
            case AST::BIN_OP_XOR:
                return "^";
            case AST::BIN_OP_LEFT_SHIFT:
                return "<<";
            case AST::BIN_OP_RIGHT_SHIFT:
                return ">>";
            case AST::BIN_OP_DIVIDE:
                return "/";
            case AST::BIN_OP_MOD:
                return "%";
            case AST::BIN_OP_POWER:
                return "**";
            case AST::UN_OP_NOT:
                return "!";
            case AST::UN_OP_BIN_NEGATION:
                return "~";
            case AST::LEX_OP_PLUS:
                return "+";
            case AST::LEX_OP_MINUS:
                return "-";
            case AST::LEX_OP_STAR:
                return "*";
            default:
                throw ParseException("expected function name");
        }
    }
    if (currentLexerToken == Lexer::TOK_BRACKET_LEFT) {
        nextLexerToken();
        if (currentLexerToken == Lexer::TOK_BRACKET_RIGHT) {
            nextLexerToken();
            if (currentLexerToken == Lexer::TOK_OP && queue->getOperation() == AST::BIN_OP_ASSIGN) {
                nextLexerToken();
                return "[]=";
            } else {
                return "[]";
            }
        }
    }
    throw ParseException("expected function name");
}

AST::Statement *Parser::parseFunctionDef() {
    nextLexerToken();
    skipTerms();
    AST::Statement *singleton(nullptr);
    std::string functionName;
    parseFunctionDefHeader(&functionName, &singleton);
    skipSpaces();
    bool hasParen = false;
    if (currentLexerToken == Lexer::TOK_PAREN_LEFT) {
        nextLexerToken();
        hasParen = true;
    }
    auto args = parseFuncDefArgs(hasParen);
    if (hasParen) {
        if (currentLexerToken != Lexer::TOK_PAREN_RIGHT) {
            throw ParseException("expected \")\"");
        }
        nextLexerToken();
    }
    auto body = parseCompStatement();
    if (currentLexerToken != Lexer::TOK_END) {
        throw ParseException("exepcted \"end\"");
    }
    nextLexerToken();
    return new AST::FunctionDef(functionName, body,
                                std::get<0>(args),
                                std::get<1>(args),
                                std::get<2>(args),
                                std::get<3>(args),
                                std::get<4>(args),
                                std::get<5>(args),
                                singleton);
}

void Parser::parseFunctionDefHeader(std::string *functionName, AST::Statement **singleton) {
    int ptr = queue->getPtr();
    try {
        ParseRewindGuard prg(this);
        *singleton = parseStatement();
        prg.release();
    } catch (ParseException &) {
        *singleton = nullptr;
        *functionName = parseFunctionName();
        return;
    }
    skipTerms();
    if (currentLexerToken != Lexer::TOK_DOT) {
        delete *singleton;
        *singleton = nullptr;
        rewindTo(ptr);
        *functionName = parseFunctionName();
        return;
    }
    nextLexerToken();
    skipTerms();

    *functionName = parseFunctionName();
}

std::string Parser::parseName(bool withEqSign) {
    std::string name;
    if (currentLexerToken == Lexer::TOK_IDENTIFIER) {
        name = queue->getStr();
        nextLexerToken();
    }
    if (currentLexerToken == Lexer::TOK_OP) {
        switch (queue->getOperation()) {
            case AST::BIN_OP_QUESTION: {
                nextLexerToken();
                return name + "?";
            }
            case AST::UN_OP_NOT: {
                nextLexerToken();
                return name + "!";
            }
            case AST::BIN_OP_ASSIGN: {
                if (!withEqSign)break;
                nextLexerToken();
                return name + "=";
            }
            default: {
                if (name.empty()) {
                    throw ParseException("expected name");
                }
                return name;
            }
        }
    }
    return name;
}

bool Parser::testName(bool withEqSign) {
    if (currentLexerToken == Lexer::TOK_IDENTIFIER)return true;
    if (currentLexerToken == Lexer::TOK_OP) {
        auto op = queue->getOperation();
        return op == AST::BIN_OP_QUESTION || op == AST::UN_OP_NOT || (withEqSign && op == AST::BIN_OP_ASSIGN);
    }
    return false;
}

} // UltraRuby
} // Parser

