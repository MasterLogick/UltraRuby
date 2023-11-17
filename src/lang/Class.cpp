#include "Class.h"
#include "BasicClasses.h"
#include "PrimaryConstants.h"

namespace UltraRuby {
namespace Lang {
Class::Class(Class *parent, const std::string &name, int size) : Object(BasicClasses::ClassClass) {
    this->name = name;
    this->parent = parent;
    this->instanceSize = size;
}
} // UltraRuby
} // Lang