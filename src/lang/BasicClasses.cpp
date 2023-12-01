#include "BasicClasses.h"
#include "PrimaryConstants.h"
#include "String.h"
#include "Symbol.h"
#include "Array.h"
#include "Hash.h"

namespace UltraRuby {
namespace Lang {
Module *BasicClasses::RootModule = nullptr;
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
Module *BasicClasses::KernelModule = nullptr;

void BasicClasses::init() {
    ClassClass = reinterpret_cast<Class *>(new char[sizeof(Class)]);

    RootModule = new Module("", nullptr);
    BasicObjectClass = new Class("BasicClass", nullptr, nullptr, sizeof(Object));
    ObjectClass = new Class("Object", BasicObjectClass, nullptr, sizeof(Object));
    ClassClass = new(ClassClass) Class("Class", ObjectClass, nullptr, sizeof(Class));
    ModuleClass = new Class("Module", ObjectClass, nullptr, sizeof(Class));
    StringClass = new Class("String", ObjectClass, nullptr, sizeof(String));
    SymbolClass = new Class("Symbol", ObjectClass, nullptr, sizeof(Symbol));
    NilClass = new Class("NilClass", ObjectClass, nullptr, sizeof(Object));
    TrueClass = new Class("TrueClass", ObjectClass, nullptr, sizeof(Object));
    FalseClass = new Class("FalseClass", ObjectClass, nullptr, sizeof(Object));
    ArrayClass = new Class("Array", ObjectClass, nullptr, sizeof(Array));
    HashClass = new Class("Hash", ObjectClass, nullptr, sizeof(Hash));
    KernelModule = new Module("Kernel", nullptr);

    RootModule->setConst(Symbol::get("BasicClass"), BasicObjectClass);
    RootModule->setConst(Symbol::get("Object"), ObjectClass);
    RootModule->setConst(Symbol::get("Class"), ClassClass);
    RootModule->setConst(Symbol::get("Module"), ModuleClass);
    RootModule->setConst(Symbol::get("String"), StringClass);
    RootModule->setConst(Symbol::get("Symbol"), SymbolClass);
    RootModule->setConst(Symbol::get("NilClass"), NilClass);
    RootModule->setConst(Symbol::get("TrueClass"), TrueClass);
    RootModule->setConst(Symbol::get("FalseClass"), FalseClass);
    RootModule->setConst(Symbol::get("Array"), ArrayClass);
    RootModule->setConst(Symbol::get("Hash"), HashClass);
    RootModule->setConst(Symbol::get("Kernel"), KernelModule);
}
} // UltraRuby
} // Lang