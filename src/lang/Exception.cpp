#include "Exception.h"

//extern "C" void __cxa_throw(void *, const void *, void *);

//extern "C" void *__cxa_allocate_exception(size_t);

namespace UltraRuby {
namespace Lang {
Lang::Object *tt() {
    try {
        throw Exception(nullptr);
    } catch (Exception &e) {
        int a = 1 + 5;
        (void) a;
        return e.getException();
    }
}
} // UltraRuby
} // Lang