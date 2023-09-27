#ifndef ULTRA_RUBY_HASH_H
#define ULTRA_RUBY_HASH_H

#include <vector>
#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Hash : public Statement {
public:
    explicit Hash(std::vector<std::pair<std::unique_ptr<Statement>, std::unique_ptr<Statement>>> hash)
            : Statement(STMT_HASH), hash(std::move(hash)) {}

private:
    std::vector<std::pair<std::unique_ptr<Statement>, std::unique_ptr<Statement>>> hash;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_HASH_H
