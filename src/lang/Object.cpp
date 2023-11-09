#include <iostream>
#include "Object.h"
#include "Class.h"
#include "Symbol.h"
#include "Array.h"
#include "Hash.h"
#include "PrimaryConstants.h"

namespace UltraRuby {
namespace Lang {
using CallType = Object *(*)(Object *, ...);

#define CallEnding(namedMap, block, args)\
if (func->hasNamedArgs()) {\
    if (func->hasBlock()) {\
        switch (func->getDirectArgsNum()) {\
            case 0:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this,namedMap, block\
                        );\
            case 1:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap, block,\
                        args[0]);\
            case 2:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap, block,\
                        args[0], args[1]);\
            case 3:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap, block,\
                        args[0], args[1], args[2]);\
            case 4:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap, block,\
                        args[0], args[1], args[2], args[3]);\
            case 5:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap, block,\
                        args[0], args[1], args[2], args[3], args[4]);\
            default:\
                return nullptr;\
        }\
    } else {\
        switch (func->getDirectArgsNum()) {\
            case 0:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap\
                        );\
            case 1:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap,\
                        args[0]);\
            case 2:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap,\
                        args[0], args[1]);\
            case 3:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap,\
                        args[0], args[1], args[2]);\
            case 4:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap,\
                        args[0], args[1], args[2], args[3]);\
            case 5:\
                return reinterpret_cast<CallType>(func->getFunction())(\
                        this, namedMap,\
                        args[0], args[1], args[2], args[3], args[4]);\
            default:\
                return nullptr;\
        }\
    }\
} else if (func->hasBlock()) {\
    switch (func->getDirectArgsNum()) {\
        case 0:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this, block\
                    );\
        case 1:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this, block,\
                    args[0]);\
        case 2:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this, block,\
                    args[0], args[1]);\
        case 3:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this, block,\
                    args[0], args[1], args[2]);\
        case 4:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this, block,\
                    args[0], args[1], args[2], args[3]);\
        case 5:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this, block,\
                    args[0], args[1], args[2], args[3], args[4]);\
        default:\
            return nullptr;\
    }\
} else {\
    switch (func->getDirectArgsNum()) {\
        case 0:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this\
                    );\
        case 1:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this,\
                    args[0]);\
        case 2:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this,\
                    args[0], args[1]);\
        case 3:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this,\
                    args[0], args[1], args[2]);\
        case 4:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this,\
                    args[0], args[1], args[2], args[3]);\
        case 5:\
            return reinterpret_cast<CallType>(func->getFunction())(\
                    this,\
                    args[0], args[1], args[2], args[3], args[4]);\
        default:\
            return nullptr;\
    }\
}

template<class... Args>
Object *Object::call(Symbol *name, Args ...oArgs) {
    constexpr char CallNumArgs = sizeof...(oArgs);
    auto *func = findFunction(name);
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        return nullptr;
    }
    if (func->getRequiredArgsTotalNum() > CallNumArgs) {
        // todo throw oArgs count mismatch exception
        return nullptr;
    }
    if (func->isSimple()) {
        if (func->getRequiredArgsTotalNum() != CallNumArgs) {
            // todo throw oArgs count mismatch exception
            return nullptr;
        }
        return reinterpret_cast<CallType>(func->getFunction())(this, oArgs...);
    }
    if (func->getDirectArgsNum() <= MaxDirectArgsLen) {
        Object *sargs[CallNumArgs] = {oArgs...};
        Object *args[MaxDirectArgsLen];
        // fill required prefix
        for (int i = 0; i < func->getRequiredArgsPrefixNum(); ++i) {
            args[i] = sargs[i];
        }
        // fill required suffix
        for (int i = -func->getRequiredArgsSuffixNum(); i < 0; ++i) {
            args[func->getDirectArgsNum() + i] = sargs[CallNumArgs + i];
        }
        // fill provided optional args
        for (int i = 0; i < func->getOptionalArgsNum() && i < CallNumArgs - func->getRequiredArgsTotalNum(); ++i) {
            args[func->getRequiredArgsPrefixNum() + i] = sargs[func->getRequiredArgsPrefixNum() + i];
        }
        // zero lacking optional args
        for (int i = CallNumArgs - func->getRequiredArgsTotalNum(); i < func->getOptionalArgsNum(); ++i) {
            args[func->getRequiredArgsPrefixNum() + i] = nullptr;
        }
        // construct variadic
        if (func->hasVariadic()) {
            auto variadicStart = func->getRequiredArgsPrefixNum() + func->getOptionalArgsNum();
            auto variadicLen = CallNumArgs - func->getDirectArgsNum() + 1;
            if (variadicLen > 0) {
                args[variadicStart] = Array::allocOnHeap(variadicLen, sargs + variadicStart);
            } else {
                args[variadicStart] = Array::allocOnHeap(0, nullptr);
            }
        }
        CallEnding(nullptr, PrimaryConstants::nilConst, args)
    } else {
        Object *sargs[CallNumArgs] = {oArgs...};
        return callV(name, CallNumArgs, sargs);
    }
}

template Object *Object::call(Symbol *);

template Object *Object::call(Symbol *, Object *);

template Object *Object::call(Symbol *, Object *, Object *);

template Object *Object::call(Symbol *, Object *, Object *, Object *);

template Object *Object::call(Symbol *, Object *, Object *, Object *, Object *);

template Object *Object::call(Symbol *, Object *, Object *, Object *, Object *, Object *);

template<class... Args>
Object *Object::callB(Symbol *name, Proc *block, Args ...oArgs) {
    constexpr char CallNumArgs = sizeof...(oArgs);
    auto *func = findFunction(name);
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        return nullptr;
    }
    if (func->getRequiredArgsTotalNum() > CallNumArgs) {
        // todo throw oArgs count mismatch exception
        return nullptr;
    }
    if (func->isSimple()) {
        if (func->getRequiredArgsTotalNum() != CallNumArgs) {
            // todo throw oArgs count mismatch exception
            return nullptr;
        }
        return reinterpret_cast<CallType>(func->getFunction())(this, oArgs...);
    }
    if (func->getDirectArgsNum() <= MaxDirectArgsLen) {
        Object *sargs[CallNumArgs] = {oArgs...};
        Object *args[MaxDirectArgsLen];
        // fill required prefix
        for (int i = 0; i < func->getRequiredArgsPrefixNum(); ++i) {
            args[i] = sargs[i];
        }
        // fill required suffix
        for (int i = -func->getRequiredArgsSuffixNum(); i < 0; ++i) {
            args[func->getDirectArgsNum() + i] = sargs[CallNumArgs + i];
        }
        // fill provided optional args
        for (int i = 0; i < func->getOptionalArgsNum() && i < CallNumArgs - func->getRequiredArgsTotalNum(); ++i) {
            args[func->getRequiredArgsPrefixNum() + i] = sargs[func->getRequiredArgsPrefixNum() + i];
        }
        // zero lacking optional args
        for (int i = CallNumArgs - func->getRequiredArgsTotalNum(); i < func->getOptionalArgsNum(); ++i) {
            args[func->getRequiredArgsPrefixNum() + i] = nullptr;
        }
        // construct variadic
        if (func->hasVariadic()) {
            auto variadicStart = func->getRequiredArgsPrefixNum() + func->getOptionalArgsNum();
            auto variadicLen = CallNumArgs - func->getDirectArgsNum() + 1;
            if (variadicLen > 0) {
                args[variadicStart] = Array::allocOnHeap(variadicLen, sargs + variadicStart);
            } else {
                args[variadicStart] = Array::allocOnHeap(0, nullptr);
            }
        }
        CallEnding(nullptr, block, args)
    } else {
        Object *sargs[CallNumArgs] = {oArgs...};
        return callBV(name, block, CallNumArgs, sargs);
    }
}

template Object *Object::callB(Symbol *, Proc *);

template Object *Object::callB(Symbol *, Proc *, Object *);

template Object *Object::callB(Symbol *, Proc *, Object *, Object *);

template Object *Object::callB(Symbol *, Proc *, Object *, Object *, Object *);

template Object *Object::callB(Symbol *, Proc *, Object *, Object *, Object *, Object *);

template Object *Object::callB(Symbol *, Proc *, Object *, Object *, Object *, Object *, Object *);

template<class... Args>
Object *Object::callN(Symbol *name, Hash *namedMap, Args ...oArgs) {
    constexpr char CallNumArgs = sizeof...(oArgs);
    auto *func = findFunction(name);
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        return nullptr;
    }
    if (!func->hasNamedArgs()) {
        if (CallNumArgs + 1 <= MaxDirectArgsLen) {
            return call(name, oArgs..., (Object *) namedMap);
        } else {
            Object *sargs[] = {oArgs..., namedMap};
            return callV(name, CallNumArgs + 1, sargs);
        }
    }
    if (func->getRequiredArgsTotalNum() > CallNumArgs) {
        // todo throw oArgs count mismatch exception
        return nullptr;
    }
    if (func->getDirectArgsNum() <= MaxDirectArgsLen) {
        Object *sargs[CallNumArgs] = {oArgs...};
        Object *args[MaxDirectArgsLen];
        // fill required prefix
        for (int i = 0; i < func->getRequiredArgsPrefixNum(); ++i) {
            args[i] = sargs[i];
        }
        // fill required suffix
        for (int i = -func->getRequiredArgsSuffixNum(); i < 0; ++i) {
            args[func->getDirectArgsNum() + i] = sargs[CallNumArgs + i];
        }
        // fill provided optional args
        for (int i = 0; i < func->getOptionalArgsNum() && i < CallNumArgs - func->getRequiredArgsTotalNum(); ++i) {
            args[func->getRequiredArgsPrefixNum() + i] = sargs[func->getRequiredArgsPrefixNum() + i];
        }
        // zero lacking optional args
        for (int i = CallNumArgs - func->getRequiredArgsTotalNum(); i < func->getOptionalArgsNum(); ++i) {
            args[func->getRequiredArgsPrefixNum() + i] = nullptr;
        }
        // construct variadic
        if (func->hasVariadic()) {
            auto variadicStart = func->getRequiredArgsPrefixNum() + func->getOptionalArgsNum();
            auto variadicLen = CallNumArgs - func->getDirectArgsNum() + 1;
            if (variadicLen > 0) {
                args[variadicStart] = Array::allocOnHeap(variadicLen, sargs + variadicStart);
            } else {
                args[variadicStart] = Array::allocOnHeap(0, nullptr);
            }
        }
        CallEnding(namedMap, PrimaryConstants::nilConst, args)
    } else {
        Object *sargs[CallNumArgs] = {oArgs...};
        return callNV(name, namedMap, CallNumArgs, sargs);
    }
}

template Object *Object::callN(Symbol *, Hash *);

template Object *Object::callN(Symbol *, Hash *, Object *);

template Object *Object::callN(Symbol *, Hash *, Object *, Object *);

template Object *Object::callN(Symbol *, Hash *, Object *, Object *, Object *);

template Object *Object::callN(Symbol *, Hash *, Object *, Object *, Object *, Object *);

template Object *Object::callN(Symbol *, Hash *, Object *, Object *, Object *, Object *, Object *);

template<class... Args>
Object *Object::callNB(Symbol *name, Hash *namedMap, Proc *block, Args ...oArgs) {
    constexpr char CallNumArgs = sizeof...(oArgs);
    auto *func = findFunction(name);
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        return nullptr;
    }
    if (!func->hasNamedArgs()) {
        if (CallNumArgs + 1 <= MaxDirectArgsLen) {
            return callB(name, block, oArgs..., (Object *) namedMap);
        } else {
            Object *sargs[] = {oArgs..., namedMap};
            return callBV(name, block, CallNumArgs + 1, sargs);
        }
    }
    if (func->getRequiredArgsTotalNum() > CallNumArgs) {
        // todo throw oArgs count mismatch exception
        return nullptr;
    }
    if (func->getDirectArgsNum() <= MaxDirectArgsLen) {
        Object *sargs[CallNumArgs] = {oArgs...};
        Object *args[MaxDirectArgsLen];
        // fill required prefix
        for (int i = 0; i < func->getRequiredArgsPrefixNum(); ++i) {
            args[i] = sargs[i];
        }
        // fill required suffix
        for (int i = -func->getRequiredArgsSuffixNum(); i < 0; ++i) {
            args[func->getDirectArgsNum() + i] = sargs[CallNumArgs + i];
        }
        // fill provided optional args
        for (int i = 0; i < func->getOptionalArgsNum() && i < CallNumArgs - func->getRequiredArgsTotalNum(); ++i) {
            args[func->getRequiredArgsPrefixNum() + i] = sargs[func->getRequiredArgsPrefixNum() + i];
        }
        // zero lacking optional args
        for (int i = CallNumArgs - func->getRequiredArgsTotalNum(); i < func->getOptionalArgsNum(); ++i) {
            args[func->getRequiredArgsPrefixNum() + i] = nullptr;
        }
        // construct variadic
        if (func->hasVariadic()) {
            auto variadicStart = func->getRequiredArgsPrefixNum() + func->getOptionalArgsNum();
            auto variadicLen = CallNumArgs - func->getDirectArgsNum() + 1;
            if (variadicLen > 0) {
                args[variadicStart] = Array::allocOnHeap(variadicLen, sargs + variadicStart);
            } else {
                args[variadicStart] = Array::allocOnHeap(0, nullptr);
            }
        }
        CallEnding(namedMap, block, args)
    } else {
        Object *sargs[CallNumArgs] = {oArgs...};
        return callNBV(name, namedMap, block, CallNumArgs, sargs);
    }
}

template Object *Object::callNB(Symbol *, Hash *, Proc *);

template Object *Object::callNB(Symbol *, Hash *, Proc *, Object *);

template Object *Object::callNB(Symbol *, Hash *, Proc *, Object *, Object *);

template Object *Object::callNB(Symbol *, Hash *, Proc *, Object *, Object *, Object *);

template Object *Object::callNB(Symbol *, Hash *, Proc *, Object *, Object *, Object *, Object *);

template Object *Object::callNB(Symbol *, Hash *, Proc *, Object *, Object *, Object *, Object *, Object *);

Symbol *Object::defineInstanceMethod(Symbol *nameSymbol, FunctionDefMeta *methodDef) {
    objectClass->getConsts().set(nameSymbol, methodDef);
    return nameSymbol;
}

Symbol *Object::defineSingletonMethod(Symbol *nameSymbol, FunctionDefMeta *methodDef) {
    singletonMethods.set(nameSymbol, methodDef);
    return nameSymbol;
}

Object *Object::defineClassInstance(Object *(*)(Object *)) {
    return nullptr;
}

const FunctionDefMeta *Object::findFunction(Symbol *name) {
    // todo maybe check that name starts with low case
    auto *v = singletonMethods.get(name);
    if (v == nullptr) {
        Class *cl = objectClass;
        while (cl != nullptr) {
            v = cl->getConsts().get(name);
            if (v != nullptr) break;
            cl = cl->getParent();
        }
        if (v == nullptr) {
            // todo search in global scope
        }
    }
    return static_cast<const FunctionDefMeta *>(v);
}

Object *Object::callV(Symbol *name, int n, Object **args) {

}

Object *Object::callBV(Symbol *name, Proc *block, int n, Object **args) {

}

Object *Object::callNV(Symbol *name, Hash *namedMap, int n, Object **args) {

}

Object *Object::callNBV(Symbol *name, Hash *namedMap, Proc *block, int n, Object **args) {

}

} // UltraRuby
} // Lang
