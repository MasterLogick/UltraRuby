#include "Heap.h"
#include "Class.h"

namespace UltraRuby {
namespace Lang {
Object *Heap::allocateNewObject(size_t size) {
    return reinterpret_cast<Object *>(new uint8_t[size]);
}
} // UltraRuby
} // Lang