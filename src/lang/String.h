#ifndef ULTRA_RUBY_LANG_STRING_H
#define ULTRA_RUBY_LANG_STRING_H

#include <string>
#include "Object.h"

namespace UltraRuby {
namespace Lang {

class String : public Object {
public:
    static String *get(const char *val);

    static String *get(const std::string &val);

    const std::string &getVal() const {
        return val;
    }

private:
    explicit String(std::string val);

    std::string val;
};

} // UltraRuby
} // Lang

#endif //ULTRA_RUBY_LANG_STRING_H
