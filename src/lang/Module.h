#ifndef ULTRA_RUBY_MODULE_H
#define ULTRA_RUBY_MODULE_H

#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Module : public Object {
public:
    Module(std::string name, Module *holder);

    const std::string &getName() const {
        return name;
    }

    HashInternal &getConsts() {
        return consts;
    }

    Module *getHolder() const {
        return holder;
    }

    Object *getConst(Symbol *nameSymbol);

    void setConst(Symbol *nameSymbol, Object *object);

    const void *findModuleSingletonMethod(Symbol *methodName);

    const void *findModuleInstanceMethod(Symbol *methodName);

protected:
    std::string name;
    HashInternal consts;
    Module *holder;
    std::vector<Module *> included;
    std::vector<Module *> extended;
    std::vector<Module *> prepended;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_MODULE_H
