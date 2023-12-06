#ifndef ULTRA_RUBY_AST_HASH_H
#define ULTRA_RUBY_AST_HASH_H

#include <vector>
#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Hash : public Statement {
public:
    Hash(std::vector<std::pair<Statement *, Statement *>> hash, int row, int col)
            : Statement(STMT_HASH, row, col), hash(std::move(hash)) {}

    const std::vector<std::pair<Statement *, Statement *>> &getHash() const {
        return hash;
    }

private:
    std::vector<std::pair<Statement *, Statement *>> hash;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_HASH_H
