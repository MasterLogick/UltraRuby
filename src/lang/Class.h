#ifndef ULTRA_RUBY_LANG_CLASS_H
#define ULTRA_RUBY_LANG_CLASS_H

#include <string>
#include "HashInternal.h"
#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Class : public Object {
public:
    Class(Object *parent, const std::string &name, int size);

    static Object *defineClass(Symbol *nameSymbol, Class **classPtr, Class *parent, Object *(*definition)(Class *));

    const HashInternal &getConsts() const {
        return consts;
    }

    const std::string &getName() const {
        return name;
    }

    Object *getParent() const {
        return parent;
    }

    int getInstanceSize() const {
        return instanceSize;
    }

private:
    HashInternal consts;
    std::string name;
    Object *parent;
    int instanceSize;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_CLASS_H
