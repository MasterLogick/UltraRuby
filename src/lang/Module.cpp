#include "Module.h"
#include "BasicClasses.h"
#include "Exception.h"
#include "Symbol.h"

namespace UltraRuby {
namespace Lang {
Module::Module(std::string name, Module *holder) :
        Object(BasicClasses::ClassClass),
        name(std::move(name)),
        holder(holder) {}

Object *Module::getConst(Symbol *nameSymbol) {
    if (auto *cons = consts.get(nameSymbol)) {
        return static_cast<Object *>(cons);
    } else {
        auto *h = holder;
        while (h) {
            if (auto *cn = static_cast<Object *>(h->consts.get(nameSymbol))) {
                return cn;
            }
            h = h->holder;
        }
    }
    throw Exception(nullptr);
    //todo throw exception
}

void Module::setConst(Symbol *nameSymbol, Object *object) {
    consts.set(nameSymbol, object);
}

const void *Module::findModuleSingletonMethod(Symbol *methodName) {
    if (singletonMethods) {
        if (auto *v = singletonMethods->get(methodName)) {
            return v;
        }
    }
    for (const auto &module: extended) {
        if (auto *v = module->findModuleInstanceMethod(methodName)) {
            return v;
        }
    }
    return nullptr;
}

const void *Module::findModuleInstanceMethod(Symbol *methodName) {
    for (const auto &module: prepended) {
        if (auto *cv = module->findModuleInstanceMethod(methodName)) {
            return cv;
        }
    }
    if (auto *v = methods.get(methodName)) {
        if ((reinterpret_cast<uintptr_t>(v) & 1) == 0) {
            return v;
        }
    }
    for (const auto &module: included) {
        if (auto *cv = module->findModuleInstanceMethod(methodName)) {
            return cv;
        }
    }
    return nullptr;
}
} // UltraRuby
} // Lang