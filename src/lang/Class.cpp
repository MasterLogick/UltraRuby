#include "Class.h"

#include <utility>
#include "BasicClasses.h"
#include "PrimaryConstants.h"
#include "Exception.h"
#include "Symbol.h"

namespace UltraRuby {
namespace Lang {
Class::Class(std::string name, Class *parent, Module *holder, int size) :
        Module(std::move(name), holder),
        parent(parent),
        instanceSize(size) {
    objectClass = BasicClasses::ClassClass;
}

const void *Class::findClassSingletonMethod(Symbol *methodName) {
    if (auto *v = findModuleSingletonMethod(methodName)) {
        return v;
    }
    return parent->findClassSingletonMethod(methodName);
}

const void *Class::findClassInstanceMethod(Symbol *methodName) {
    if (auto *v = findModuleInstanceMethod(methodName)) {
        return v;
    }
    return parent->findClassInstanceMethod(methodName);
}
} // UltraRuby
} // Lang