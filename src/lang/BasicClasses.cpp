#include "BasicClasses.h"
#include "PrimaryConstants.h"
#include "String.h"
#include "Symbol.h"
#include "Array.h"
#include "Hash.h"

namespace UltraRuby {
namespace Lang {
Class *BasicClasses::BasicObjectClass = nullptr;
Class *BasicClasses::ObjectClass = nullptr;
Class *BasicClasses::ClassClass = nullptr;
Class *BasicClasses::StringClass = nullptr;
Class *BasicClasses::SymbolClass = nullptr;
Class *BasicClasses::NilClass = nullptr;
Class *BasicClasses::TrueClass = nullptr;
Class *BasicClasses::FalseClass = nullptr;
Class *BasicClasses::ArrayClass = nullptr;
Class *BasicClasses::HashClass = nullptr;

void BasicClasses::init() {
    BasicObjectClass = new Class(nullptr, "BasicClass", sizeof(Object));
    ObjectClass = new Class(BasicObjectClass, "Object", sizeof(Object));
    ClassClass = new Class(ObjectClass, "Class", sizeof(Class));
    StringClass = new Class(ObjectClass, "String", sizeof(String));
    SymbolClass = new Class(ObjectClass, "Symbol", sizeof(Symbol));
    NilClass = new Class(ObjectClass, "NilClass", sizeof(Object));
    TrueClass = new Class(ObjectClass, "TrueClass", sizeof(Object));
    FalseClass = new Class(ObjectClass, "FalseClass", sizeof(Object));
    ArrayClass = new Class(ObjectClass, "Array", sizeof(Array));
    HashClass = new Class(ObjectClass, "Hash", sizeof(Hash));
}
} // UltraRuby
} // Lang