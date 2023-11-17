#ifndef ULTRA_RUBY_PRIMARYCONSTANTS_H
#define ULTRA_RUBY_PRIMARYCONSTANTS_H

#include "Object.h"

namespace UltraRuby {
namespace Lang {

class PrimaryConstants {
public:
    static Object NilConst;
    static Object TrueConst;
    static Object FalseConst;
    static Object GlobalScope;
    static void init();

};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_PRIMARYCONSTANTS_H
