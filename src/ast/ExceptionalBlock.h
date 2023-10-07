#ifndef ULTRA_RUBY_AST_EXCEPTIONALBLOCK_H
#define ULTRA_RUBY_AST_EXCEPTIONALBLOCK_H

#include <memory>
#include "Statement.h"
#include "Block.h"

namespace UltraRuby {
namespace AST {

class ExceptionalBlock : public Statement {
public:
    ExceptionalBlock(
            Block *mainBlock,
            std::vector<std::tuple<AST::Statement *, std::string, AST::Block *>> rescues,
            Block *elseBlock,
            Block *ensureBlock
    ) :
            Statement(STMT_EXCEPTIONAL_BLOCK),
            mainBlock(std::move(mainBlock)),
            rescues(std::move(rescues)),
            elseBlock(std::move(elseBlock)),
            ensureBlock(std::move(ensureBlock)) {}

    inline Block *getMainBlock() const {
        return mainBlock;
    }

    inline const std::vector<std::tuple<AST::Statement *, std::string, AST::Block *>> &getRescues() const {
        return rescues;
    }

    inline Block *getElseBlock() const {
        return elseBlock;
    }

    inline Block *getEnsureBlock() const {
        return ensureBlock;
    }

private:
    Block *mainBlock;
    std::vector<std::tuple<AST::Statement *, std::string, AST::Block *>> rescues;
    Block *elseBlock;
    Block *ensureBlock;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_EXCEPTIONALBLOCK_H
