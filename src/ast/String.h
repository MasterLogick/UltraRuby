#ifndef ULTRA_RUBY_STRING_H
#define ULTRA_RUBY_STRING_H

#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class String : public Statement {
public:
    explicit String(std::string str) : Statement(STMT_STRING), str(std::move(str)) {}

    inline const std::string &getStr() const {
        return str;
    }

private:
    std::string str;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_STRING_H
