#ifndef ULTRA_RUBY_LANG_CLASS_H
#define ULTRA_RUBY_LANG_CLASS_H

#include <string>
#include "HashInternal.h"
#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Class : public Object {
public:
    Class(Class *parent, const std::string &name, int size);

    HashInternal &getConsts() {
        return consts;
    }

    const std::string &getName() const {
        return name;
    }

    Class *getParent() const {
        return parent;
    }

    int getInstanceSize() const {
        return instanceSize;
    }

private:
    HashInternal consts;
    std::string name;
    Class *parent;
    int instanceSize;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_CLASS_H
