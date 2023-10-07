#ifndef ULTRA_RUBY_AST_HASH_H
#define ULTRA_RUBY_AST_HASH_H

#include <vector>
#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Hash : public Statement {
public:
    explicit Hash(std::vector<std::pair<Statement *, Statement *>> hash)
            : Statement(STMT_HASH), hash(std::move(hash)) {}

    inline const std::vector<std::pair<Statement *, Statement *>> &getHash() const {
        return hash;
    }

private:
    std::vector<std::pair<Statement *, Statement *>> hash;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_HASH_H
