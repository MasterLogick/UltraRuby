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
            std::unique_ptr<Block> mainBlock,
            std::vector<std::tuple<std::unique_ptr<AST::Statement>, std::string, std::unique_ptr<AST::Block>>> rescues,
            std::unique_ptr<Block> elseBlock,
            std::unique_ptr<Block> ensureBlock
    ) :
            Statement(STMT_EXCEPTIONAL_BLOCK),
            mainBlock(std::move(mainBlock)),
            rescues(std::move(rescues)),
            elseBlock(std::move(elseBlock)),
            ensureBlock(std::move(ensureBlock)) {}

    inline const std::unique_ptr<Block> &getMainBlock() const {
        return mainBlock;
    }

    inline const std::vector<std::tuple<std::unique_ptr<AST::Statement>, std::string, std::unique_ptr<AST::Block>>> &
    getRescues() const {
        return rescues;
    }

    inline const std::unique_ptr<Block> &getElseBlock() const {
        return elseBlock;
    }

    inline const std::unique_ptr<Block> &getEnsureBlock() const {
        return ensureBlock;
    }

private:
    std::unique_ptr<Block> mainBlock;
    std::vector<std::tuple<std::unique_ptr<AST::Statement>, std::string, std::unique_ptr<AST::Block>>> rescues;
    std::unique_ptr<Block> elseBlock;
    std::unique_ptr<Block> ensureBlock;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_EXCEPTIONALBLOCK_H
