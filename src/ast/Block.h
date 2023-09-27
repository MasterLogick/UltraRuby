#ifndef ULTRA_RUBY_BLOCK_H
#define ULTRA_RUBY_BLOCK_H

#include <memory>
#include <vector>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Block : public Statement {
public:
    explicit Block(std::vector<std::unique_ptr<AST::Statement>> list)
            : Statement(STMT_BLOCK), list(std::move(list)) {}

private:
    std::vector<std::unique_ptr<AST::Statement>> list;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_BLOCK_H
