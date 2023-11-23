#include "Hash.h"
#include "BasicClasses.h"
#include "Heap.h"

namespace UltraRuby {
namespace Lang {
Hash *Hash::allocOnHeap(int n, Object **pairs) {
    auto *a = reinterpret_cast<Hash *>(Heap::allocateNewObject(sizeof(Hash)));
    new(a)Hash();
    for (int i = 0; i < n; ++i) {
        a->impl.set(pairs[2 * i], pairs[2 * i + 1]);
    }
    return a;
}

Hash::Hash() : Object(BasicClasses::HashClass) {

}
} // UltraRuby
} // Lang