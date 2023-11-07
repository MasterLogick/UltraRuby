#ifndef ULTRA_RUBY_FUNCTIONDEFMETA_H
#define ULTRA_RUBY_FUNCTIONDEFMETA_H

namespace UltraRuby {
namespace Lang {

class FunctionDefMeta {
public:
    FunctionDefMeta(
            char requiredArgsPrefixNum,
            char optionalArgsNum,
            char variadicPos,
            char requiredArgsSuffixNum,
            bool namedArgsPresent,
            bool capturesBlock,
            void *function
    ) :
            requiredArgsPrefixNum(requiredArgsPrefixNum),
            optionalArgsNum(optionalArgsNum),
            variadicPos(variadicPos),
            requiredArgsSuffixNum(requiredArgsSuffixNum),
            flags((namedArgsPresent ? 1 : 0) | (capturesBlock ? 2 : 0)),
            function(function),
            requiredArgsTotalNum(requiredArgsPrefixNum + requiredArgsSuffixNum),
            argsTotalNum(requiredArgsPrefixNum + optionalArgsNum + requiredArgsSuffixNum + (namedArgsPresent ? 1 : 0) +
                         (capturesBlock ? 1 : 0)) {}

    char getRequiredArgsPrefixNum() const {
        return requiredArgsPrefixNum;
    }

    char getOptionalArgsNum() const {
        return optionalArgsNum;
    }

    char getVariadicPos() const {
        return variadicPos;
    }

    char getRequiredArgsSuffixNum() const {
        return requiredArgsSuffixNum;
    }

    bool hasNamedArgs() const {
        return flags & 1;
    }

    bool isCapturesBlock() const {
        return flags & 2;
    }

    void *getFunction() const {
        return function;
    }

    char getRequiredArgsTotalNum() const {
        return requiredArgsTotalNum;
    }

    char getArgsTotalNum() const {
        return argsTotalNum;
    }

    char getFlags() const {
        return flags;
    }

private:
    void *function;
    char requiredArgsPrefixNum;
    char optionalArgsNum;
    char variadicPos;
    char requiredArgsSuffixNum;
    char requiredArgsTotalNum;

    char argsTotalNum;
    char flags;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_FUNCTIONDEFMETA_H
