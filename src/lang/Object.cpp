#include <cassert>
#include <cstring>
#include "Object.h"
#include "Class.h"
#include "Symbol.h"
#include "Hash.h"
#include "PrimaryConstants.h"
#include "Exception.h"
#include "Proc.h"

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
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findFunction(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == 0xf) {
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
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findFunction(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == 0xf) {
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
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findFunction(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == 0xf) {
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

Object *Object::callNV(Symbol *name, Hash *namedMap, int n, Object **args) {
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findFunction(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == 0xf) {
        auto *savedProc = currentProc;
        currentProc = PrimaryConstants::NilConst;
        Object *retVal;
        if (func->hasNamedArgs) {
            auto *args1 = new Object *[n + 1];
            memcpy(args1, args, n);
            args1[n] = namedMap;
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

Object *Object::callNBV(Symbol *name, Hash *namedMap, Proc *block, int n, Object **args) {
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findFunction(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == 0xf) {
        auto *savedProc = currentProc;
        currentProc = block;
        Object *retVal;
        if (func->hasNamedArgs) {
            auto *args1 = new Object *[n + 1];
            memcpy(args1, args, n);
            args1[n] = namedMap;
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

Symbol *Object::defineInstanceMethod(Symbol *nameSymbol, void *function, int argc, bool hasBlock, bool hasNamedArgs) {
    if (argc > MaxDirectArgsLen || (hasBlock || hasNamedArgs && argc != -1)) {
        // todo throw format mismatch exception
        throw Exception(nullptr);
    }
    objectClass->getConsts().set(nameSymbol, new FunctionDefMeta(function, argc, hasBlock, hasNamedArgs));
    return nameSymbol;
}

Symbol *Object::defineSingletonMethod(Symbol *nameSymbol, void *function, int argc, bool hasBlock, bool hasNamedArgs) {
    if (argc > MaxDirectArgsLen || (hasBlock || hasNamedArgs && argc != -1)) {
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

const void *Object::findFunction(Symbol *name) {
    void *v;
    if (singletonMethods) {
        v = singletonMethods->get(name);
        if (v != nullptr) {
            return v;
        }
    }
    if (objectClass == BasicClasses::ClassClass) {
        auto *cl = reinterpret_cast<Class *>(this)->getParent();
        while (cl != nullptr) {
            auto *singMet = cl->getSingletonMethods();
            if (singMet) {
                v = singMet->get(name);
                if (v) {
                    return v;
                }
            }
            cl = cl->getParent();
        }
    } else {
        Class *cl = objectClass;
        while (cl != nullptr) {
            v = cl->getConsts().get(name);
            if (v != nullptr) break;
            cl = cl->getParent();
        }
    }
    return nullptr;
}

Object *Object::defineClass(Symbol *nameSymbol, Class *parent, Object *(*definition)(Class *)) {
    if (this == PrimaryConstants::GlobalScope) {
        return BasicClasses::RootClass->defineClass(nameSymbol, parent, definition);
    }
    assert(objectClass == BasicClasses::ClassClass && "class definition in method body");
    auto *c = static_cast<Class *>(this);
    if (auto *v = c->getConsts().get(nameSymbol)) {
        return definition(static_cast<Class *>(v));
    }
    auto *v = new Class(parent, nameSymbol->getSym(), sizeof(Object));
    c->setConst(nameSymbol, v);
    return definition(v);
}

Object *Object::defineModule(Symbol *nameSymbol, Object *(*definition)(Class *)) {
    throw Exception(nullptr);
}

} // UltraRuby
} // Lang
