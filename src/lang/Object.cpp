#include <cassert>
#include <cstring>
#include "Object.h"
#include "Class.h"
#include "Symbol.h"
#include "Hash.h"
#include "PrimaryConstants.h"
#include "Exception.h"
#include "Proc.h"
#include "Module.h"

namespace UltraRuby {
namespace Lang {
template<class...Args>
using CallType = Object *(*)(Object *, Args...);
using CallVType = Object *(*)(Object *, int, Object **);

thread_local Object *Object::currentProc = nullptr;

struct FunctionDefMeta {
    void *function;
    int argc;
    bool hasBlock;
    bool hasNamedArgs;
};

template<class... Args>
Object *Object::call(Symbol *name, Args ...oArgs) {
    constexpr char CallNumArgs = sizeof...(oArgs);
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findMethod(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == -1) {
        auto *savedProc = currentProc;
        currentProc = PrimaryConstants::NilConst;
        Object *retVal;
        if (func->hasNamedArgs) {
            Object *args[CallNumArgs + 1] = {oArgs..., Hash::allocOnHeap(0, nullptr)};
            retVal = reinterpret_cast<CallVType>(func->function)(this, CallNumArgs, args);
        } else {
            Object *args[CallNumArgs] = {oArgs...};
            retVal = reinterpret_cast<CallVType>(func->function)(this, CallNumArgs, args);
        }
        currentProc = savedProc;
        return retVal;
    } else {
        assert(!func->hasNamedArgs && !func->hasBlock);
        if (func->argc != CallNumArgs) {
            //todo throw wrong number of arguments (given reqArgs, expected CallNumArgs) (ArgumentError)
            throw Exception(nullptr);
        }
        auto *savedProc = currentProc;
        currentProc = PrimaryConstants::NilConst;
        auto *retVal = reinterpret_cast<CallType<Args...>>(func->function)
                (this, oArgs...);
        currentProc = savedProc;
        return retVal;
    }
}

template Object *Object::call(Symbol *);

template Object *Object::call(Symbol *, Object *);

template Object *Object::call(Symbol *, Object *, Object *);

template Object *Object::call(Symbol *, Object *, Object *, Object *);

template Object *Object::call(Symbol *, Object *, Object *, Object *, Object *);

template Object *Object::call(Symbol *, Object *, Object *, Object *, Object *, Object *);

Object *Object::callV(Symbol *name, int n, Object **args) {
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findMethod(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == -1) {
        auto *savedProc = currentProc;
        currentProc = PrimaryConstants::NilConst;
        Object *retVal;
        if (func->hasNamedArgs) {
            auto *args1 = new Object *[n + 1];
            memcpy(args1, args, n);
            args1[n] = Hash::allocOnHeap(0, nullptr);
            retVal = reinterpret_cast<CallVType>(func->function)(this, n, args1);
        } else {
            retVal = reinterpret_cast<CallVType>(func->function)(this, n, args);
        }
        currentProc = savedProc;
        return retVal;
    } else {
        if (n != func->argc) {
            //todo throw wrong number of arguments (given n, expected func->argc) (ArgumentError)
            throw Exception(nullptr);
        }
        auto *savedProc = currentProc;
        currentProc = PrimaryConstants::NilConst;
        Object *retVal;
        switch (n) {
            case 0:
                retVal = reinterpret_cast<CallType<>>(func->function)
                        (this);
                break;
            case 1:
                retVal = reinterpret_cast<CallType<Object *>>(func->function)
                        (this, args[0]);
                break;
            case 2:
                retVal = reinterpret_cast<CallType<Object *, Object *>>(func->function)
                        (this, args[0], args[1]);
                break;
            case 3:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2]);
                break;
            case 4:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3]);
                break;
            case 5:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3], args[4]);
                break;
            default:
                std::terminate();
        }
        static_assert(MaxDirectArgsLen == 5, "implement more");
        currentProc = savedProc;
        return retVal;
    }
}

Object *Object::callBV(Symbol *name, Proc *block, int n, Object **args) {
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findMethod(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == -1) {
        auto *savedProc = currentProc;
        currentProc = block;
        Object *retVal;
        if (func->hasNamedArgs) {
            auto *args1 = new Object *[n + 1];
            memcpy(args1, args, n);
            args1[n] = Hash::allocOnHeap(0, nullptr);
            retVal = reinterpret_cast<CallVType>(func->function)(this, n, args1);
        } else {
            retVal = reinterpret_cast<CallVType>(func->function)(this, n, args);
        }
        currentProc = savedProc;
        return retVal;
    } else {
        if (n != func->argc) {
            //todo throw wrong number of arguments (given n, expected func->argc) (ArgumentError)
            throw Exception(nullptr);
        }
        auto *savedProc = currentProc;
        currentProc = block;
        Object *retVal;
        switch (n) {
            case 0:
                retVal = reinterpret_cast<CallType<>>(func->function)
                        (this);
                break;
            case 1:
                retVal = reinterpret_cast<CallType<Object *>>(func->function)
                        (this, args[0]);
                break;
            case 2:
                retVal = reinterpret_cast<CallType<Object *, Object *>>(func->function)
                        (this, args[0], args[1]);
                break;
            case 3:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2]);
                break;
            case 4:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3]);
                break;
            case 5:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3], args[4]);
                break;
            default:
                std::terminate();
        }
        static_assert(MaxDirectArgsLen == 5, "implement more");
        currentProc = savedProc;
        return retVal;
    }
}

Object *Object::callNV(Symbol *name, int n, Object **args) {
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findMethod(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == -1) {
        auto *savedProc = currentProc;
        currentProc = PrimaryConstants::NilConst;
        Object *retVal = reinterpret_cast<CallVType>(func->function)(this, func->hasNamedArgs ? n : n + 1, args);
        currentProc = savedProc;
        return retVal;
    } else {
        if (n != func->argc) {
            //todo throw wrong number of arguments (given n, expected func->argc) (ArgumentError)
            throw Exception(nullptr);
        }
        auto *savedProc = currentProc;
        currentProc = PrimaryConstants::NilConst;
        Object *retVal;
        switch (n) {
            case 0:
                retVal = reinterpret_cast<CallType<>>(func->function)
                        (this);
                break;
            case 1:
                retVal = reinterpret_cast<CallType<Object *>>(func->function)
                        (this, args[0]);
                break;
            case 2:
                retVal = reinterpret_cast<CallType<Object *, Object *>>(func->function)
                        (this, args[0], args[1]);
                break;
            case 3:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2]);
                break;
            case 4:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3]);
                break;
            case 5:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3], args[4]);
                break;
            default:
                std::terminate();
        }
        static_assert(MaxDirectArgsLen == 5, "implement more");
        currentProc = savedProc;
        return retVal;
    }
}

Object *Object::callNBV(Symbol *name, Proc *block, int n, Object **args) {
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findMethod(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == -1) {
        auto *savedProc = currentProc;
        currentProc = block;
        Object *retVal = reinterpret_cast<CallVType>(func->function)(this, func->hasNamedArgs ? n : n + 1, args);
        currentProc = savedProc;
        return retVal;
    } else {
        if (n != func->argc) {
            //todo throw wrong number of arguments (given n, expected func->argc) (ArgumentError)
            throw Exception(nullptr);
        }
        auto *savedProc = currentProc;
        currentProc = PrimaryConstants::NilConst;
        Object *retVal;
        switch (n) {
            case 0:
                retVal = reinterpret_cast<CallType<>>(func->function)
                        (this);
                break;
            case 1:
                retVal = reinterpret_cast<CallType<Object *>>(func->function)
                        (this, args[0]);
                break;
            case 2:
                retVal = reinterpret_cast<CallType<Object *, Object *>>(func->function)
                        (this, args[0], args[1]);
                break;
            case 3:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2]);
                break;
            case 4:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3]);
                break;
            case 5:
                retVal = reinterpret_cast<CallType<Object *, Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3], args[4]);
                break;
            default:
                std::terminate();
        }
        static_assert(MaxDirectArgsLen == 5, "implement more");
        currentProc = savedProc;
        return retVal;
    }
}

Object *Object::callForward(Symbol *name, int n, Object **args) {
    //todo named args map may be remain unused and undetected if callee do not use it
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findMethod(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == -1) {
        return reinterpret_cast<CallVType>(func->function)(this, n, args);
    } else {
        if (n != func->argc) {
            //todo throw wrong number of arguments (given n, expected func->argc) (ArgumentError)
            throw Exception(nullptr);
        }
        switch (n) {
            case 0:
                return reinterpret_cast<CallType<>>(func->function)
                        (this);
            case 1:
                return reinterpret_cast<CallType<Object *>>(func->function)
                        (this, args[0]);
            case 2:
                return reinterpret_cast<CallType<Object *, Object *>>(func->function)
                        (this, args[0], args[1]);
            case 3:
                return reinterpret_cast<CallType<Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2]);
            case 4:
                return reinterpret_cast<CallType<Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3]);
            case 5:
                return reinterpret_cast<CallType<Object *, Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3], args[4]);
            default:
                std::terminate();
        }
        static_assert(MaxDirectArgsLen == 5, "implement more");
    }
}

Symbol *Object::defineInstanceMethod(Symbol *nameSymbol, void *function, int argc, bool hasBlock, bool hasNamedArgs) {
    if (argc > MaxDirectArgsLen || ((hasBlock || hasNamedArgs) && argc != -1)) {
        // todo throw format mismatch exception
        throw Exception(nullptr);
    }
    if (objectClass == BasicClasses::ClassClass) {
        reinterpret_cast<Class *>(this)->getConsts().set(nameSymbol,
                                                         new FunctionDefMeta(function, argc, hasBlock, hasNamedArgs));
    } else {
        objectClass->getConsts().set(nameSymbol, new FunctionDefMeta(function, argc, hasBlock, hasNamedArgs));
    }
    return nameSymbol;
}

Symbol *Object::defineSingletonMethod(Symbol *nameSymbol, void *function, int argc, bool hasBlock, bool hasNamedArgs) {
    if (argc > MaxDirectArgsLen || ((hasBlock || hasNamedArgs) && argc != -1)) {
        // todo throw format mismatch exception
        throw Exception(nullptr);
    }
    if (singletonMethods == nullptr) {
        singletonMethods = new HashInternal();
    }
    singletonMethods->set(nameSymbol, new FunctionDefMeta(function, argc, hasBlock, hasNamedArgs));
    return nameSymbol;
}

Object *Object::defineClassInstance(Object *(*definition)(Object *)) {
    //todo rebind object to new subclass and invoke definition
    throw Exception(nullptr);
}

const void *Object::findMethod(Symbol *name) {
    void *v;
    if (singletonMethods) {
        v = singletonMethods->get(name);
        if (v != nullptr) {
            return v;
        }
    }
    if (objectClass == BasicClasses::ClassClass) {
        return reinterpret_cast<Class *>(this)->findClassSingletonMethod(name);
    } else if (objectClass == BasicClasses::ModuleClass) {
        return reinterpret_cast<Module *>(this)->findModuleSingletonMethod(name);
    } else {
        return objectClass->findClassInstanceMethod(name);
    }
}

Object *Object::defineClass(Symbol *nameSymbol, Class *parent, Object *(*definition)(Class *)) {
    //todo move to Class class
    if (this == PrimaryConstants::GlobalScope) {
        return BasicClasses::RootModule->defineClass(nameSymbol, parent, definition);
    }
    assert((objectClass == BasicClasses::ClassClass || objectClass == BasicClasses::ModuleClass) &&
           "disallow class definition in method body");
    auto *c = reinterpret_cast<Module *>(this);
    if (auto *v = static_cast<Object *>(c->getConsts().get(nameSymbol))) {
        if (v->objectClass != BasicClasses::ClassClass) {
            //todo throw exception nameSymbol is not a class (TypeError)
            throw Exception(nullptr);
        }
        return definition(reinterpret_cast<Class *>(v));
    }
    auto *v = new Class(nameSymbol->getSym(), parent, reinterpret_cast<Module *>(this), sizeof(Object));
    c->setConst(nameSymbol, v);
    return definition(v);
}

Object *Object::defineModule(Symbol *nameSymbol, Object *(*definition)(Module *)) {
    //todo move to Module class
    if (this == PrimaryConstants::GlobalScope) {
        return BasicClasses::RootModule->defineModule(nameSymbol, definition);
    }
    assert((objectClass == BasicClasses::ClassClass || objectClass == BasicClasses::ModuleClass) &&
           "disallow module definition in method body");

    auto *c = reinterpret_cast<Module *>(this);
    if (auto *v = static_cast<Object *>(c->getConsts().get(nameSymbol))) {
        if (v->objectClass != BasicClasses::ModuleClass) {
            //todo throw exception nameSymbol is not a module (TypeError)
            throw Exception(nullptr);
        }
        return definition(reinterpret_cast<Module *>(v));
    }
    auto *v = new Module(nameSymbol->getSym(), reinterpret_cast<Module *>(this));
    c->setConst(nameSymbol, v);
    return definition(v);
}

} // UltraRuby
} // Lang
