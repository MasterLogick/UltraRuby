#ifndef ULTRA_RUBY_PROTOTYPE_H
#define ULTRA_RUBY_PROTOTYPE_H

#include <string>
#include <vector>

namespace UltraRuby {
namespace AST {

class Prototype {
public:
    Prototype(const std::string &name, const std::vector<std::string> &args)
            : name(name), args(args) {}

    inline const std::string &getName() const {
        return name;
    }

    inline const std::vector<std::string> &getArgs() const {
        return args;
    }

private:
    std::string name;
    std::vector<std::string> args;
};

} // UltraRuby
} // AST

#endif //ULTRA_RUBY_PROTOTYPE_H
