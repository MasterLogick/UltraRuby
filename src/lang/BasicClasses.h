#ifndef ULTRA_RUBY_LANG_BASICCLASSES_H
#define ULTRA_RUBY_LANG_BASICCLASSES_H

#include "Class.h"

namespace UltraRuby {
namespace Lang {

class BasicClasses {
public:
    static Class *RootClass;
    static Class *BasicObjectClass;
    static Class *ObjectClass;
    static Class *ClassClass;
    static Class *ModuleClass;
    static Class *StringClass;
    static Class *SymbolClass;
    static Class *NilClass;
    static Class *TrueClass;
    static Class *FalseClass;
    static Class *ArrayClass;
    static Class *HashClass;

    static void init();
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_BASICCLASSES_H
