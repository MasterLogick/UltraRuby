#include "BasicClasses.h"
#include "PrimaryConstants.h"
#include "String.h"
#include "Symbol.h"

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

void BasicClasses::init() {
    BasicObjectClass = reinterpret_cast<Class *>(new char[sizeof(Class)]);
    BasicObjectClass = new(BasicObjectClass) Class(PrimaryConstants::nilConst, "BasicClass", sizeof(Object));
    ObjectClass = new Class(BasicObjectClass, "Object", sizeof(Object));
    ClassClass = new Class(ObjectClass, "Class", sizeof(Class));
    StringClass = new Class(ObjectClass, "String", sizeof(String));
    SymbolClass = new Class(ObjectClass, "Symbol", sizeof(Symbol));
    NilClass = new Class(ObjectClass, "NilClass", sizeof(Object));
    TrueClass = new Class(ObjectClass, "TrueClass", sizeof(Object));
    FalseClass = new Class(ObjectClass, "FalseClass", sizeof(Object));
}
} // UltraRuby
} // Lang