#ifndef ULTRA_RUBY_AST_EXCEPTIONALBLOCK_H
#define ULTRA_RUBY_AST_EXCEPTIONALBLOCK_H

#include <memory>
#include "Statement.h"
#include "Block.h"
#include "Rescue.h"

namespace UltraRuby {
namespace AST {

class ExceptionalBlock : public Statement {
public:
    ExceptionalBlock(
            Block *mainBlock,
            std::vector<Rescue *> rescues,
            Block *ensureBlock
    ) :
            Statement(STMT_EXCEPTIONAL_BLOCK),
            mainBlock(mainBlock),
            rescues(std::move(rescues)),
            ensureBlock(ensureBlock) {}

    inline Block *getMainBlock() const {
        return mainBlock;
    }

    inline const std::vector<Rescue *> &getRescues() const {
        return rescues;
    }

    inline Block *getEnsureBlock() const {
        return ensureBlock;
    }

private:
    Block *mainBlock;
    std::vector<Rescue *> rescues;
    Block *ensureBlock;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_EXCEPTIONALBLOCK_H
