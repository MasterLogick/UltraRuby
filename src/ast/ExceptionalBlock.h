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
            Block *ensureBlock,
            int row,
            int col
    ) :
            Statement(STMT_EXCEPTIONAL_BLOCK, row, col),
            mainBlock(mainBlock),
            rescues(std::move(rescues)),
            ensureBlock(ensureBlock) {}

    Block *getMainBlock() const {
        return mainBlock;
    }

    const std::vector<Rescue *> &getRescues() const {
        return rescues;
    }

    Block *getEnsureBlock() const {
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
