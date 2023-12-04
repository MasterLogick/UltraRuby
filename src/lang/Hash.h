#ifndef ULTRA_RUBY_LANG_HASH_H
#define ULTRA_RUBY_LANG_HASH_H

#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Hash : public Object {
public:
    static Hash *allocOnHeap(int n, Object **pairs);

    HashInternal &getImpl() {
        return impl;
    }

private:
    Hash();

    HashInternal impl;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_HASH_H
