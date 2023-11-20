#include "BasicClasses.h"
#include "PrimaryConstants.h"
#include "String.h"
#include "Symbol.h"
#include "Array.h"
#include "Hash.h"

namespace UltraRuby {
namespace Lang {
Class *BasicClasses::RootClass = nullptr;
Class *BasicClasses::BasicObjectClass = nullptr;
Class *BasicClasses::ObjectClass = nullptr;
Class *BasicClasses::ClassClass = nullptr;
Class *BasicClasses::ModuleClass = nullptr;
Class *BasicClasses::StringClass = nullptr;
Class *BasicClasses::SymbolClass = nullptr;
Class *BasicClasses::NilClass = nullptr;
Class *BasicClasses::TrueClass = nullptr;
Class *BasicClasses::FalseClass = nullptr;
Class *BasicClasses::ArrayClass = nullptr;
Class *BasicClasses::HashClass = nullptr;

void BasicClasses::init() {
    ClassClass = reinterpret_cast<Class *>(new char[sizeof(Class)]);
    RootClass = new Class(nullptr, "", -1);

    BasicObjectClass = new Class(nullptr, "BasicClass", sizeof(Object));
    ObjectClass = new Class(BasicObjectClass, "Object", sizeof(Object));
    ClassClass = new(ClassClass) Class(ObjectClass, "Class", sizeof(Class));
    ModuleClass = new Class(ObjectClass, "Module", sizeof(Class));
    StringClass = new Class(ObjectClass, "String", sizeof(String));
    SymbolClass = new Class(ObjectClass, "Symbol", sizeof(Symbol));
    NilClass = new Class(ObjectClass, "NilClass", sizeof(Object));
    TrueClass = new Class(ObjectClass, "TrueClass", sizeof(Object));
    FalseClass = new Class(ObjectClass, "FalseClass", sizeof(Object));
    ArrayClass = new Class(ObjectClass, "Array", sizeof(Array));
    HashClass = new Class(ObjectClass, "Hash", sizeof(Hash));

    RootClass->setConst(Symbol::get("BasicClass"), BasicObjectClass);
    RootClass->setConst(Symbol::get("Object"), ObjectClass);
    RootClass->setConst(Symbol::get("Class"), ClassClass);
    RootClass->setConst(Symbol::get("Module"), ModuleClass);
    RootClass->setConst(Symbol::get("String"), StringClass);
    RootClass->setConst(Symbol::get("Symbol"), SymbolClass);
    RootClass->setConst(Symbol::get("NilClass"), NilClass);
    RootClass->setConst(Symbol::get("TrueClass"), TrueClass);
    RootClass->setConst(Symbol::get("FalseClass"), FalseClass);
    RootClass->setConst(Symbol::get("Array"), ArrayClass);
    RootClass->setConst(Symbol::get("Hash"), HashClass);
}
} // UltraRuby
} // Lang