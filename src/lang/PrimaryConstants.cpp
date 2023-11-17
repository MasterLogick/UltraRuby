#include "PrimaryConstants.h"
#include "BasicClasses.h"
#include "Object.h"

namespace UltraRuby {
namespace Lang {
Object PrimaryConstants::NilConst(nullptr);
Object PrimaryConstants::TrueConst(nullptr);
Object PrimaryConstants::FalseConst(nullptr);
Object PrimaryConstants::GlobalScope(nullptr);

void PrimaryConstants::init() {
    // well ...
    NilConst.~Object();
    TrueConst.~Object();
    FalseConst.~Object();
    GlobalScope.~Object();
    new (&NilConst) Object(BasicClasses::NilClass);
    new (&TrueConst) Object(BasicClasses::TrueClass);
    new (&FalseConst) Object(BasicClasses::FalseClass);
    new (&GlobalScope) Object(BasicClasses::ObjectClass);
}
} // UltraRuby
} // Lang