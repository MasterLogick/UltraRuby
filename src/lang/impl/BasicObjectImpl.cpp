#include "BasicObjectImpl.h"
#include "../BasicClasses.h"
#include "../Symbol.h"
#include "../Heap.h"
#include "../PrimaryConstants.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {

void BasicObjectImpl::loadImpl() {
    auto initializePtr = &BasicObjectImpl::initialize;
    BasicClasses::BasicObjectClass->defineInstanceMethod(Symbol::get("initialize"),
                                                         *reinterpret_cast<void **>(&initializePtr), 0, false, false);
    ClassImpl::loadImpl();
}

Object *BasicObjectImpl::initialize() {
    return PrimaryConstants::NilConst;
}

Symbol *BasicObjectImpl::ClassImpl::initializeSymbol = nullptr;

void BasicObjectImpl::ClassImpl::loadImpl() {
    initializeSymbol = Symbol::get("initialize");
    auto s = &ClassImpl::self_new;
    BasicClasses::BasicObjectClass->defineSingletonMethod(Symbol::get("new"),
                                                          *reinterpret_cast<void **>(&s), -1, true, true);
}

Object *BasicObjectImpl::ClassImpl::self_new(int argc, Object **argv) {
    auto *obj = Heap::allocateNewObject(instanceSize);
    new(obj)Object(this);
    obj->callForward(initializeSymbol, argc, argv);
    return obj;
}
} // UltraRuby
} // Lang
} // Impl