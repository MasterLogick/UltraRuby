#include "HashInternal.h"
#include "Object.h"

namespace UltraRuby {
namespace Lang {
HashInternal::HashInternal() {

}

void HashInternal::set(Object *key, void *val) {
    impl[key] = val;
}

void *HashInternal::get(Object *key) {
    return impl[key];
}

void HashInternal::remove(Object *key) {
    impl.erase(key);
}
} // UltraRuby
} // Lang