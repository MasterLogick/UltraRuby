#ifndef ULTRA_RUBY_BASICOBJECTIMPL_H
#define ULTRA_RUBY_BASICOBJECTIMPL_H

#include "../Object.h"
#include "../Class.h"
#include "../Array.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {

class BasicObjectImpl : private Object {
public:
    static void loadImpl();

private:
    class ClassImpl : private Class {
    public:
        static void loadImpl();

    private:
        static Symbol *initializeSymbol;

        Object *self_new(int argc, Object **argv);
    };

    Object *initialize();
};

} // UltraRuby
} // Lang
} // Impl

#endif //ULTRA_RUBY_BASICOBJECTIMPL_H
