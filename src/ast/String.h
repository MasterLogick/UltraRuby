#ifndef ULTRA_RUBY_AST_STRING_H
#define ULTRA_RUBY_AST_STRING_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class String : public Statement {
public:
    String(std::string str, int row, int col) : Statement(STMT_STRING, row, col), str(std::move(str)) {}

    const std::string &getStr() const {
        return str;
    }

private:
    std::string str;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_AST_STRING_H
