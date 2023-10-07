#ifndef ULTRA_RUBY_LANG_OBJECT_H
#define ULTRA_RUBY_LANG_OBJECT_H

#include <vector>
#include "HashInternal.h"

namespace UltraRuby {
namespace Lang {
class Class;

class HashInternal;

class Symbol;

class Object {
public:
    Object(Class *objectClass) : objectClass(objectClass) {}

    Object *call0(Symbol *name);

    Object *call1(Symbol *name, Object *arg1);

    Object *call2(Symbol *name, Object *arg1, Object *arg2);

    Object *callV(Symbol *name, Object *args);

    Symbol *defineInstanceMethod(Symbol *nameSymbol, int minArgs, int maxArgs, void *method);

    Symbol *defineSingletonMethod(Symbol *nameSymbol, int minArgs, int maxArgs, void *method);

    Object *defineClassInstance(Object *(*)(Object *));

    Class *getObjectClass() const {
        return objectClass;
    }

    void setObjectClass(Class *oClass) {
        Object::objectClass = oClass;
    }

    const HashInternal &getSingletonMethods() const {
        return singletonMethods;
    }

    const HashInternal &getAdditionalInstanceVariables() const {
        return additionalInstanceVariables;
    }

private:
    Class *objectClass;
    HashInternal singletonMethods;
    HashInternal additionalInstanceVariables;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_OBJECT_H
