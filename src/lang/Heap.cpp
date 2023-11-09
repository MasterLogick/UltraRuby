#include "Heap.h"
#include "Class.h"

namespace UltraRuby {
namespace Lang {
Object *Heap::allocateNewObject(Class *oClass) {
    auto *arr = new char[oClass->getInstanceSize()];
    return new(arr)Object(oClass);
}
} // UltraRuby
} // Lang