#include "Array.h"
#include "Heap.h"
#include "BasicClasses.h"

namespace UltraRuby {
namespace Lang {
Array *Array::allocOnHeap(int size, Object **arr) {
    auto *a = reinterpret_cast<Array *>(Heap::allocateNewObject(BasicClasses::ArrayClass));
    new(&a->impl) std::vector<Object *>();
    a->impl.reserve(size);
    for (int i = 0; i < size; ++i) {
        a->impl.push_back(arr[i]);
    }
    return a;
}
} // UltraRuby
} // Lang