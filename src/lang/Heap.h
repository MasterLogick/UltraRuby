#ifndef ULTRA_RUBY_LANG_HEAP_H
#define ULTRA_RUBY_LANG_HEAP_H

#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Heap {
public:
    static Object *allocateNewObject(Class *oClass);
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_HEAP_H
