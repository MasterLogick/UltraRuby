#include "Class.h"

#include <utility>
#include "BasicClasses.h"
#include "PrimaryConstants.h"
#include "Exception.h"
#include "Symbol.h"

namespace UltraRuby {
namespace Lang {
Class::Class(Class *parent, std::string name, int size) :
        Object(BasicClasses::ClassClass),
        name(std::move(name)),
        parent(parent),
        instanceSize(size) {}

Object *Class::getConst(Symbol *nameSymbol) {
    if (auto *v = consts.get(nameSymbol)) {
        return static_cast<Object *>(v);
    }
    throw Exception(nullptr);
    //todo throw exception
}

void Class::setConst(Symbol *nameSymbol, Object *object) {
    consts.set(nameSymbol, object);
}
} // UltraRuby
} // Lang