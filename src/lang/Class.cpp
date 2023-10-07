#include "Class.h"
#include "BasicClasses.h"

namespace UltraRuby {
namespace Lang {
Object *Class::defineClass(Symbol *nameSymbol, Class **classPtr, Class *parent, Object *(*definition)(Class *)) {
    return *classPtr;
}

Class::Class(Object *parent, const std::string &name, int size) : Object(BasicClasses::ClassClass) {
    this->name = name;
    this->parent = parent;
    this->instanceSize = size;
}
} // UltraRuby
} // Lang