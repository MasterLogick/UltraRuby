#ifndef ULTRA_RUBY_RESCUE_H
#define ULTRA_RUBY_RESCUE_H

#include <string>
#include <utility>
#include "Statement.h"
#include "Block.h"

namespace UltraRuby {
namespace AST {
class Rescue {
public:
    Rescue(AST::Statement *rescueClass, std::string varName, AST::Block *handler) :
            rescueClass(rescueClass), varName(std::move(varName)), handler(handler) {}

    AST::Statement *rescueClass;
    std::string varName;
    AST::Block *handler;
};
} // UltraRuby
} // AST

#endif //ULTRA_RUBY_RESCUE_H
