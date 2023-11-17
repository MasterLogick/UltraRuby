#ifndef ULTRA_RUBY_LANG_OBJECT_H
#define ULTRA_RUBY_LANG_OBJECT_H

#include <vector>
#include "HashInternal.h"
#include "FunctionDefMeta.h"

namespace UltraRuby {
namespace Lang {
class Class;

class HashInternal;

class Symbol;

class FunctionDefMeta;

class Hash;

class Proc;

class Object {
public:
    static constexpr int ObjectClassOffset = 0;

    explicit Object(Class *objectClass) : objectClass(objectClass) {}

    static constexpr int MaxDirectArgsLen = 5;

    template<class... Args>
    Object *call(Symbol *name, Args ...oArgs);

    Object *callV(Symbol *name, int n, Object **args);

    template<class... Args>
    Object *callB(Symbol *name, Proc *block, Args ...args);

    Object *callBV(Symbol *name, Proc *block, int n, Object **args);

    template<class... Args>
    Object *callN(Symbol *name, Hash *namedMap, Args ...args);

    Object *callNV(Symbol *name, Hash *namedMap, int n, Object **args);

    template<class... Args>
    Object *callNB(Symbol *name, Hash *namedMap, Proc *block, Args ...args);

    Object *callNBV(Symbol *name, Hash *namedMap, Proc *block, int n, Object **args);


    Symbol *defineInstanceMethod(Symbol *nameSymbol, FunctionDefMeta *methodDef);

    Symbol *defineSingletonMethod(Symbol *nameSymbol, FunctionDefMeta *methodDef);

    Object *defineClassInstance(Object *(*)(Object *));

    Object *defineClass(Symbol *nameSymbol, Class *parent, Object *(*definition)(Class *));

    Object *defineModule(Symbol *nameSymbol, Object *(*definition)(Class *));

    Object *getConst(Symbol *nameSymbol);

    Class *getObjectClass() const {
        return objectClass;
    }

private:

    const HashInternal &getSingletonMethods() const {
        return singletonMethods;
    }

    const HashInternal &getAdditionalInstanceVariables() const {
        return additionalInstanceVariables;
    }

    const FunctionDefMeta *findFunction(Symbol *name);

    Class *objectClass;
    HashInternal singletonMethods;
    HashInternal additionalInstanceVariables;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_OBJECT_H
