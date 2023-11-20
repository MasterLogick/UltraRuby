#include "PrimaryConstants.h"
#include "BasicClasses.h"
#include "Object.h"

namespace UltraRuby {
namespace Lang {
Object *PrimaryConstants::NilConst = nullptr;
Object *PrimaryConstants::TrueConst = nullptr;
Object *PrimaryConstants::FalseConst = nullptr;
Object *PrimaryConstants::GlobalScope = nullptr;

void PrimaryConstants::init() {
    NilConst = new Object(BasicClasses::NilClass);
    TrueConst = new Object(BasicClasses::TrueClass);
    FalseConst = new Object(BasicClasses::FalseClass);
    GlobalScope = new Object(BasicClasses::ObjectClass);
}
} // UltraRuby
} // Lang