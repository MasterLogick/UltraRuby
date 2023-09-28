#ifndef ULTRA_RUBY_FUNCDEFARGS_H
#define ULTRA_RUBY_FUNCDEFARGS_H

#include <string>
#include <map>
#include <memory>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class FuncDefArgs {
public:
    FuncDefArgs(std::map<std::string, std::unique_ptr<Statement>> args) : args(std::move(args)) {}

    inline const std::map<std::string, std::unique_ptr<Statement>> &getArgs() const {
        return args;
    }

private:
    std::map<std::string, std::unique_ptr<Statement>> args;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_FUNCDEFARGS_H
