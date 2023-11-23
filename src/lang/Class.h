#ifndef ULTRA_RUBY_LANG_CLASS_H
#define ULTRA_RUBY_LANG_CLASS_H

#include <string>
#include "HashInternal.h"
#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Class : public Object {
public:
    Class(Class *parent, std::string name, int size);

    const std::string &getName() const {
        return name;
    }

    HashInternal &getConsts() {
        return consts;
    }

    Class *getParent() const {
        return parent;
    }

    int getInstanceSize() const {
        return instanceSize;
    }

    Object *getConst(Symbol *nameSymbol);

    void setConst(Symbol *nameSymbol, Object *object);

protected:
    std::string name;
    HashInternal consts;
    Class *parent;
    int instanceSize;


};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_CLASS_H
