#ifndef ULTRA_RUBY_PRIMARYCONSTANTS_H
#define ULTRA_RUBY_PRIMARYCONSTANTS_H

#include "Object.h"

namespace UltraRuby {
namespace Lang {

class PrimaryConstants {
public:
    static Object *nilConst;
    static Object *trueConst;
    static Object *falseConst;

    static void init();

};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_PRIMARYCONSTANTS_H
