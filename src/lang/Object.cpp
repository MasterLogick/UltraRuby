#include <iostream>
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
using CallBVType = Object *(*)(Object *, Object *, int, Object **);
using CallNVType = Object *(*)(Object *, Object *, int, Object **);
using CallNBVType = Object *(*)(Object *, Object *, Object *, int, Object **);

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
        Object *args[CallNumArgs] = {oArgs...};
        if (func->hasBlock)
            if (func->hasNamedArgs)
                return reinterpret_cast<CallNBVType>(func->function)
                        (this, Hash::allocOnHeap(0, nullptr), PrimaryConstants::NilConst, CallNumArgs, args);
            else
                return reinterpret_cast<CallBVType>(func->function)
                        (this, PrimaryConstants::NilConst, CallNumArgs, args);
        else if (func->hasNamedArgs)
            return reinterpret_cast<CallNVType>(func->function)
                    (this, Hash::allocOnHeap(0, nullptr), CallNumArgs, args);
        else
            return reinterpret_cast<CallVType>(func->function)
                    (this, CallNumArgs, args);
    } else {
        assert(!func->hasNamedArgs && !func->hasBlock);
        if (func->argc != CallNumArgs) {
            //todo throw wrong number of arguments (given reqArgs, expected CallNumArgs) (ArgumentError)
            throw Exception(nullptr);
        }
        return reinterpret_cast<CallType<Args...>>(func->function)
                (this, oArgs...);
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
        if (func->hasBlock) {
            if (func->hasNamedArgs) {
                return reinterpret_cast<CallNBVType>(func->function)
                        (this, Hash::allocOnHeap(0, nullptr), PrimaryConstants::NilConst, n, args);
            } else {
                auto *args1 = new Object *[n + 1];
                memcpy(args1, args, n);
                args1[n] = Hash::allocOnHeap(0, nullptr);
                return reinterpret_cast<CallBVType>(func->function)(this, PrimaryConstants::NilConst, n + 1, args1);
            }
        } else {
            assert(func->hasNamedArgs);
            return reinterpret_cast<CallNVType>(func->function)(this, Hash::allocOnHeap(0, nullptr), n, args);
        }
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

Object *Object::callBV(Symbol *name, Proc *block, int n, Object **args) {
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findFunction(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == 0xf) {
        if (func->hasBlock) {
            if (func->hasNamedArgs) {
                return reinterpret_cast<CallNBVType>(func->function)(this, Hash::allocOnHeap(0, nullptr), block, n,
                                                                     args);
            } else {
                auto *args1 = new Object *[n + 1];
                memcpy(args1, args, n);
                args1[n] = Hash::allocOnHeap(0, nullptr);
                return reinterpret_cast<CallBVType>(func->function)(this, block, n + 1, args1);
            }
        } else {
            assert(func->hasNamedArgs);
            return reinterpret_cast<CallNVType>(func->function)(this, Hash::allocOnHeap(0, nullptr), n, args);
        }
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

Object *Object::callNV(Symbol *name, Hash *namedMap, int n, Object **args) {
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findFunction(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == 0xf) {
        if (func->hasBlock) {
            if (func->hasNamedArgs) {
                return reinterpret_cast<CallNBVType>(func->function)(this, namedMap, PrimaryConstants::NilConst, n,
                                                                     args);
            } else {
                auto *args1 = new Object *[n + 1];
                memcpy(args1, args, n);
                args1[n] = namedMap;
                return reinterpret_cast<CallBVType>(func->function)(this, PrimaryConstants::NilConst, n + 1, args1);
            }
        } else {
            assert(func->hasNamedArgs);
            return reinterpret_cast<CallNVType>(func->function)(this, namedMap, n, args);
        }
    } else {
        if (n + 1 != func->argc) {
            //todo throw wrong number of arguments (given n, expected func->argc) (ArgumentError)
            throw Exception(nullptr);
        }
        switch (n + 1) {
            case 1:
                return reinterpret_cast<CallType<Object *>>(func->function)
                        (this, namedMap);
            case 2:
                return reinterpret_cast<CallType<Object *, Object *>>(func->function)
                        (this, args[0], namedMap);
            case 3:
                return reinterpret_cast<CallType<Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], namedMap);
            case 4:
                return reinterpret_cast<CallType<Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], namedMap);
            case 5:
                return reinterpret_cast<CallType<Object *, Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3], namedMap);
            default:
                std::terminate();
        }
    }
}

Object *Object::callNBV(Symbol *name, Hash *namedMap, Proc *block, int n, Object **args) {
    auto *func = reinterpret_cast<const FunctionDefMeta *>(findFunction(name));
    if (func == nullptr) {
        // todo try find function using `respond_to?`
        throw Exception(nullptr);
    }
    if (func->argc == 0xf) {
        if (func->hasBlock) {
            if (func->hasNamedArgs) {
                return reinterpret_cast<CallNBVType>(func->function)(this, namedMap, block, n, args);
            } else {
                auto *args1 = new Object *[n + 1];
                memcpy(args1, args, n);
                args1[n] = namedMap;
                return reinterpret_cast<CallBVType>(func->function)(this, block, n + 1, args1);
            }
        } else {
            assert(func->hasNamedArgs);
            return reinterpret_cast<CallNVType>(func->function)(this, namedMap, n, args);
        }
    } else {
        if (n + 1 != func->argc) {
            //todo throw wrong number of arguments (given n, expected func->argc) (ArgumentError)
            throw Exception(nullptr);
        }
        switch (n + 1) {
            case 1:
                return reinterpret_cast<CallType<Object *>>(func->function)
                        (this, namedMap);
            case 2:
                return reinterpret_cast<CallType<Object *, Object *>>(func->function)
                        (this, args[0], namedMap);
            case 3:
                return reinterpret_cast<CallType<Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], namedMap);
            case 4:
                return reinterpret_cast<CallType<Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], namedMap);
            case 5:
                return reinterpret_cast<CallType<Object *, Object *, Object *, Object *, Object *>>(func->function)
                        (this, args[0], args[1], args[2], args[3], namedMap);
            default:
                std::terminate();
        }
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
