#ifndef ULTRA_RUBY_DEFINITIONNAME_H
#define ULTRA_RUBY_DEFINITIONNAME_H

#include <vector>
#include <string>
#include "Statement.h"

namespace UltraRuby {
namespace AST {

class DefinitionName {
public:
    explicit DefinitionName(std::vector<std::string> identifier, bool isFull)
            : identifier(std::move(identifier)), isFull_(isFull) {}

    const std::vector<std::string> &getIdentifier() const {
        return identifier;
    }

    bool isFull() const {
        return isFull_;
    }

private:
    std::vector<std::string> identifier;
    bool isFull_;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_DEFINITIONNAME_H
