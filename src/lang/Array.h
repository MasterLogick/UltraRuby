#ifndef ULTRA_RUBY_LANG_ARRAY_H
#define ULTRA_RUBY_LANG_ARRAY_H

#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Array : public Object {
public:
    static Array *allocOnHeap(int size, Object **arr);

private:
    std::vector<Object *> impl;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_ARRAY_H
