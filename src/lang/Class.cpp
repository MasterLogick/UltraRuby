#include "Class.h"

namespace UltraRuby {
namespace Lang {
Object *Class::defineClass(Class **classPtr, Symbol *nameSymbol, Object *(*definition)(Class *)) {
    return *classPtr;
}
} // UltraRuby
} // Lang