#ifndef ULTRA_RUBY_LANG_OBJECT_H
#define ULTRA_RUBY_LANG_OBJECT_H

#include <vector>
#include "HashInternal.h"

namespace UltraRuby {
namespace Lang {
class Class;

class HashInternal;

class Symbol;

class Hash;

class Proc;

class Object {
public:
    static constexpr int ObjectClassOffset = 0;

    explicit Object(Class *objectClass) :
            objectClass(objectClass),
            singletonMethods(nullptr) {}

    static constexpr int MaxDirectArgsLen = 5;

    template<class... Args>
    Object *call(Symbol *name, Args ...oArgs);

    Object *callV(Symbol *name, int n, Object **args);

    Object *callBV(Symbol *name, Proc *block, int n, Object **args);

    Object *callNV(Symbol *name, int n, Object **args);

    Object *callNBV(Symbol *name, Proc *block, int n, Object **args);

    Object *callForward(Symbol *name, int n, Object **args);

    Symbol *defineInstanceMethod(Symbol *nameSymbol, void *function, int argc, bool hasBlock, bool hasNamedArgs);

    Symbol *defineSingletonMethod(Symbol *nameSymbol, void *function, int argc, bool hasBlock, bool hasNamedArgs);

    Object *defineClassInstance(Object *(*)(Object *));

    Object *defineClass(Symbol *nameSymbol, Class *parent, Object *(*definition)(Class *));

    Object *defineModule(Symbol *nameSymbol, Object *(*definition)(Class *));

    Class *getObjectClass() const {
        return objectClass;
    }

    HashInternal *getSingletonMethods() const {
        return singletonMethods;
    }

    static thread_local Object *currentProc;

protected:
    const void *findFunction(Symbol *name);

    Class *objectClass;
    HashInternal *singletonMethods;
    HashInternal instanceVariables;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_OBJECT_H
