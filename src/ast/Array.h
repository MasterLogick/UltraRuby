#ifndef ULTRA_RUBY_AST_ARRAY_H
#define ULTRA_RUBY_AST_ARRAY_H

#include <memory>
#include <vector>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class Array : public Statement {
public:
    Array(std::vector<Statement *> elements, int row, int col) :
            Statement(STMT_ARRAY, row, col), elements(std::move(elements)) {}

    const std::vector<Statement *> &getElements() const {
        return elements;
    }

private:
    std::vector<Statement *> elements;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_ARRAY_H
