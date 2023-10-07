#ifndef ULTRA_RUBY_AST_BLOCK_H
#define ULTRA_RUBY_AST_BLOCK_H

#include <memory>
#include <vector>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Block : public Statement {
public:
    explicit Block(std::vector<AST::Statement *> list) : Statement(STMT_BLOCK), list(std::move(list)) {}

    inline const std::vector<AST::Statement *> &getList() const {
        return list;
    }

private:
    std::vector<AST::Statement *> list;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_BLOCK_H
