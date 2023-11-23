#include "BasicObjectImpl.h"
#include "../BasicClasses.h"
#include "../Symbol.h"
#include "../Heap.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {

void BasicObjectImpl::loadImpl() {
    ClassImpl::loadImpl();
}

void BasicObjectImpl::ClassImpl::loadImpl() {
    auto s = &ClassImpl::self_new_impl;
    BasicClasses::BasicObjectClass->defineSingletonMethod(Symbol::get("new"),
                                                          *reinterpret_cast<void **>(&s), -1, true, true);
}

Object *BasicObjectImpl::ClassImpl::self_new_impl(Hash *h, Proc *b, int argc, Object **args) {
    auto *obj = Heap::allocateNewObject(this);
    //todo check that initialize accepts namedMap
    obj->callNBV(Symbol::get("initialize"), h, b, argc, args);
    return obj;
}
} // UltraRuby
} // Lang
} // Impl