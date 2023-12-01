#include <functional>
#include "BasicObjectImpl.h"
#include "../BasicClasses.h"
#include "../Symbol.h"
#include "../Heap.h"
#include "../PrimaryConstants.h"
#include "../Exception.h"
#include "../String.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {

void BasicObjectImpl::loadImpl() {
    Class *basicObjectClass = BasicClasses::BasicObjectClass;
    auto initializePtr = &BasicObjectImpl::initialize;
    basicObjectClass->defineInstanceMethod(Symbol::get("initialize"),
                                           *reinterpret_cast<void **>(&initializePtr), 0, false, false);
    auto negationPtr = &BasicObjectImpl::negation;
    basicObjectClass->defineInstanceMethod(Symbol::get("!"), *reinterpret_cast<void **>(&negationPtr), 0,
                                           false, false);
    auto equalPtr = &BasicObjectImpl::equal;
    basicObjectClass->defineInstanceMethod(Symbol::get("=="), *reinterpret_cast<void **>(&equalPtr), 1,
                                           false, false);
    auto not_equalPtr = &BasicObjectImpl::not_equal;
    basicObjectClass->defineInstanceMethod(Symbol::get("!="), *reinterpret_cast<void **>(&not_equalPtr),
                                           1, false, false);
    auto idPtr = &BasicObjectImpl::id;
    basicObjectClass->defineInstanceMethod(Symbol::get("__id__"), *reinterpret_cast<void **>(&idPtr), 0,
                                           false, false);
    auto sendPtr = &BasicObjectImpl::send;
    basicObjectClass->defineInstanceMethod(Symbol::get("__send__"), *reinterpret_cast<void **>(&sendPtr),
                                           -1, true, true);
    auto samePtr = &BasicObjectImpl::same;
    basicObjectClass->defineInstanceMethod(Symbol::get("equal?"), *reinterpret_cast<void **>(&samePtr), 1,
                                           false, false);
    auto instance_evalPtr = &BasicObjectImpl::instance_eval;
    basicObjectClass->defineInstanceMethod(Symbol::get("instance_eval"),
                                           *reinterpret_cast<void **>(&instance_evalPtr), -1, true, true);
    auto instance_execPtr = &BasicObjectImpl::instance_exec;
    basicObjectClass->defineInstanceMethod(Symbol::get("instance_exec"),
                                           *reinterpret_cast<void **>(&instance_execPtr), -1, true, true);
    ClassImpl::loadImpl();
}

Object *BasicObjectImpl::initialize() {
    return PrimaryConstants::NilConst;
}

Object *BasicObjectImpl::negation() {
    return PrimaryConstants::FalseConst;
}

Object *BasicObjectImpl::equal(Object *rhs) {
    return this == rhs ? PrimaryConstants::TrueConst : PrimaryConstants::FalseConst;
}

Object *BasicObjectImpl::not_equal(Object *rhs) {
    return this != rhs ? PrimaryConstants::TrueConst : PrimaryConstants::FalseConst;
}

Object *BasicObjectImpl::id() {
    //todo make better hash function
    union {
        void *ptr;
        unsigned int val[2];
    } un{};
    un.ptr = this;
    auto a = un.val[0] ^ un.val[1];
    un.val[0] = a;
    un.val[1] = 0;
    return static_cast<Object *>(un.ptr);
}

Object *BasicObjectImpl::send(int argc, Object **argv) {
    if (argc < 1) {
        throw Exception(nullptr);
    }
    Object *maybeSym = argv[0];
    Symbol *sym;
    if (maybeSym->getObjectClass() == BasicClasses::SymbolClass) {
        sym = reinterpret_cast<Symbol *>(maybeSym);
    } else if (maybeSym->getObjectClass() == BasicClasses::StringClass) {
        sym = Symbol::get(reinterpret_cast<String *>(maybeSym)->getVal());
    } else {
        // todo throw maybeSym is not a symbol nor a string (TypeError)
        throw Exception(nullptr);
    }
    return callForward(sym, argc - 1, argv + 1);
}

Object *BasicObjectImpl::same(Object *rhs) {
    return this == rhs ? PrimaryConstants::TrueConst : PrimaryConstants::FalseConst;
}

Object *BasicObjectImpl::instance_eval(int argc, Object **argv) {
    throw Exception(nullptr);
}

Object *BasicObjectImpl::instance_exec(int argc, Object **argv) {
    throw Exception(nullptr);
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