#ifndef ULTRA_RUBY_FUNCTIONDEFMETA_H
#define ULTRA_RUBY_FUNCTIONDEFMETA_H

namespace UltraRuby {
namespace Lang {

class FunctionDefMeta {
public:
    FunctionDefMeta(
            char requiredArgsPrefixNum,
            char optionalArgsNum,
            bool hasVariadic,
            char requiredArgsSuffixNum,
            bool namedArgsPresent,
            bool hasBlock,
            void *function
    ) :
            requiredArgsPrefixNum(requiredArgsPrefixNum),
            optionalArgsNum(optionalArgsNum),
            requiredArgsSuffixNum(requiredArgsSuffixNum),
            flags((namedArgsPresent ? HasNamedArgsFlag : 0) | (hasBlock ? HasBlockFlag : 0) |
                  (optionalArgsNum == 0 && !hasVariadic && requiredArgsSuffixNum == 0 && !namedArgsPresent &&
                   !hasBlock ? SimpleFunctionFlag : 0) | (hasVariadic ? HasVariadicFlag : 0)),
            function(function),
            requiredArgsTotalNum(requiredArgsPrefixNum + requiredArgsSuffixNum),
            directArgsNum(
                    requiredArgsPrefixNum + optionalArgsNum + requiredArgsSuffixNum + (hasVariadic ? 1 : 0)) {}

    char getRequiredArgsPrefixNum() const {
        return requiredArgsPrefixNum;
    }

    char getOptionalArgsNum() const {
        return optionalArgsNum;
    }

    bool hasVariadic() const {
        return flags & HasVariadicFlag;
    }

    char getRequiredArgsSuffixNum() const {
        return requiredArgsSuffixNum;
    }

    bool hasNamedArgs() const {
        return flags & HasNamedArgsFlag;
    }

    bool hasBlock() const {
        return flags & HasBlockFlag;
    }

    bool isSimple() const {
        return flags & SimpleFunctionFlag;
    }

    void *getFunction() const {
        return function;
    }

    char getRequiredArgsTotalNum() const {
        return requiredArgsTotalNum;
    }

    char getDirectArgsNum() const {
        return directArgsNum;
    }

    char getFlags() const {
        return flags;
    }

private:
    static constexpr char HasNamedArgsFlag = 1;
    static constexpr char HasBlockFlag = 2;
    static constexpr char SimpleFunctionFlag = 4;
    static constexpr char HasVariadicFlag = 8;
    void *function;
    char requiredArgsPrefixNum;
    char optionalArgsNum;
    char requiredArgsSuffixNum;
    char requiredArgsTotalNum;
    char directArgsNum;
    char flags;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_FUNCTIONDEFMETA_H
