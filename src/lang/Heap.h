#ifndef ULTRA_RUBY_LANG_HEAP_H
#define ULTRA_RUBY_LANG_HEAP_H

#include <cstddef>
#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Heap {
public:
    /**
     * Allocates size bytes chunk for Object instance
     * @param size
     * @return
     */
    static Object *allocateNewObject(size_t size);
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_HEAP_H
