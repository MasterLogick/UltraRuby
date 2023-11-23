#include "Array.h"
#include "Heap.h"
#include "BasicClasses.h"

namespace UltraRuby {
namespace Lang {
Array *Array::allocOnHeap(int size, Object **arr) {
    auto *a = reinterpret_cast<Array *>(Heap::allocateNewObject(sizeof(Array)));
    new(a)Array();
    a->impl.reserve(size);
    for (int i = 0; i < size; ++i) {
        a->impl.push_back(arr[i]);
    }
    return a;
}

Array::Array() : Object(BasicClasses::ArrayClass) {

}
} // UltraRuby
} // Lang