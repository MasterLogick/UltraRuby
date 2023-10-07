#include "PrimaryConstants.h"
#include "BasicClasses.h"
#include "Object.h"

namespace UltraRuby {
namespace Lang {
Object *PrimaryConstants::nilConst = nullptr;
Object *PrimaryConstants::trueConst = nullptr;
Object *PrimaryConstants::falseConst = nullptr;

void PrimaryConstants::init() {
    nilConst = new Object(BasicClasses::NilClass);
    trueConst = new Object(BasicClasses::TrueClass);
    falseConst = new Object(BasicClasses::FalseClass);
}
} // UltraRuby
} // Lang