#include <iostream>
#include "Object.h"
#include "Class.h"
#include "Symbol.h"
#include "Array.h"

namespace UltraRuby {
namespace Lang {
using CallType = Object *(*)(Object *, Object *, Object *, Object *, Object *, Object *);
using CallType0 = Object *(*)(Object *);
using CallType1 = Object *(*)(Object *, Object *);
using CallType2 = Object *(*)(Object *, Object *, Object *);
using CallType3 = Object *(*)(Object *, Object *, Object *, Object *);
using CallType4 = Object *(*)(Object *, Object *, Object *, Object *, Object *);
using CallType5 = Object *(*)(Object *, Object *, Object *, Object *, Object *, Object *);
using VariadicCallType = Object *(*)(Object *, Array *);

Object *Object::call0(Symbol *name) {
    auto *func = findFunction(name);
    if (func == nullptr) {
        // todo throw exception. (maybe even in findFunction)
        return nullptr;
    }
    if (func->getRequiredArgsTotalNum() != 0) {
        // todo throw args count mismatch exception
        return nullptr;
    }
    // todo save and clear lambda ptr in thread local storage
    if (func->getVariadicPos() != -1) {
        //todo variadic pos may be not null
        if (func->getVariadicPos() != 0) {
            // todo
            return nullptr;
        }
        auto *arr = Array::allocOnHeap(0);
        return reinterpret_cast<VariadicCallType>(func->getFunction())(this, arr);
    }
    if (func->getOptionalArgsNum() != 0) {
        switch (func->getOptionalArgsNum()) {
            case 1:
                return reinterpret_cast<CallType1>(func->getFunction())(this, nullptr);
            case 2:
                return reinterpret_cast<CallType2>(func->getFunction())(this, nullptr, nullptr);
            case 3:
                return reinterpret_cast<CallType3>(func->getFunction())(this, nullptr, nullptr, nullptr);
            case 4:
                return reinterpret_cast<CallType4>(func->getFunction())(this, nullptr, nullptr, nullptr, nullptr);
            case 5:
                return reinterpret_cast<CallType5>(func->getFunction())(this, nullptr, nullptr, nullptr, nullptr,
                                                                        nullptr);
        }
    }
    return reinterpret_cast<CallType0>(func->getFunction())(this);
}

Object *Object::call1(Symbol *name, Object *arg1) {
    auto *func = findFunction(name);
    if (func == nullptr) {
        // todo throw exception. (maybe even in findFunction)
        return nullptr;
    }
    if (func->getRequiredArgsTotalNum() > 1 || func->getArgsTotalNum() == 0) {
        // todo throw args count mismatch exception
        return nullptr;
    }
    // todo save and clear lambda ptr in thread local storage
    if (func->getVariadicPos() != -1) {
        auto *arr = Array::allocOnHeap(1, arg1);
        return reinterpret_cast<VariadicCallType>(func->getFunction())(this, arr);
    }
    return reinterpret_cast<CallType>(func->getFunction())(this, arg1, nullptr, nullptr, nullptr, nullptr);
}

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

FunctionDefMeta *Object::findFunction(Symbol *name) {
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
    return static_cast<FunctionDefMeta *>(v);
}
} // UltraRuby
} // Lang