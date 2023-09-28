#ifndef ULTRA_RUBY_ARRAY_H
#define ULTRA_RUBY_ARRAY_H

#include <memory>
#include <vector>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Array : public Statement {
public:
    Array(std::vector<std::unique_ptr<Statement>> elements) : Statement(STMT_ARRAY), elements(std::move(elements)) {}

    inline const std::vector<std::unique_ptr<Statement>> &getElements() const {
        return elements;
    }

private:
    std::vector<std::unique_ptr<Statement>> elements;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_ARRAY_H
