#ifndef ULTRA_RUBY_LANG_CLASS_H
#define ULTRA_RUBY_LANG_CLASS_H

#include <string>
#include "HashInternal.h"
#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Class : public Object {
public:
    static Object *defineClass(Class **classPtr, Symbol *nameSymbol, Object *(*definition)(Class *));

private:
    HashInternal *consts;
    std::string name;
    Class *parent;
    int instanceSize;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_CLASS_H
