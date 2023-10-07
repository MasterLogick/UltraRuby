#ifndef ULTRA_RUBY_LANG_ARRAY_H
#define ULTRA_RUBY_LANG_ARRAY_H

#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Array : public Object {
public:
    static Array *allocOnHeap(int size, ...);

private:
    std::vector<Object *> arrImpl;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_ARRAY_H
