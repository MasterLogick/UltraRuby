#ifndef ULTRA_RUBY_EXCEPTION_H
#define ULTRA_RUBY_EXCEPTION_H

#include <exception>
#include "Object.h"

namespace UltraRuby {
namespace Lang {

class Exception : std::exception {
public:
    Exception(Object *exception) : exception(exception) {}

    inline Object *getException() const {
        return exception;
    }

private:
    Object *exception;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_EXCEPTION_H
