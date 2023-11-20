#ifndef ULTRA_RUBY_LANG_HASHINTERNAL_H
#define ULTRA_RUBY_LANG_HASHINTERNAL_H

#include <map>

namespace UltraRuby {
namespace Lang {
class Object;

class HashInternal {
public:
    HashInternal();

    void set(Object *key, void *val);

    void *get(Object *key);

    bool has(Object *key);

    void remove(Object *key);

private:
    std::map<Object *, void *> impl;

};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_HASHINTERNAL_H
