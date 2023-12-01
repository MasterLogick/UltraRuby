#ifndef ULTRA_RUBY_LANG_CLASS_H
#define ULTRA_RUBY_LANG_CLASS_H

#include <string>
#include "HashInternal.h"
#include "Object.h"
#include "Module.h"
#include "Module.h"

namespace UltraRuby {
namespace Lang {

class Class : public Module {
public:
    Class(std::string name, Class *parent, Module *holder, int size);

    Class *getParent() const {
        return parent;
    }

    int getInstanceSize() const {
        return instanceSize;
    }

    const void *findClassSingletonMethod(Symbol *methodName);

    const void *findClassInstanceMethod(Symbol *methodName);

protected:
    Class *parent;
    int instanceSize;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_CLASS_H
