#ifndef ULTRA_RUBY_MODULE_H
#define ULTRA_RUBY_MODULE_H

#include "Class.h"

namespace UltraRuby {
namespace Lang {

class Module : public Object {
public:
    static Object *defineModule(Symbol *nameSymbol, Module **classPtr, Object *(*definition)(Module *));

private:

};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_MODULE_H
