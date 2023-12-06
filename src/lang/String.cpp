#include "String.h"
#include "Heap.h"
#include "BasicClasses.h"

namespace UltraRuby {
namespace Lang {
String *String::get(const std::string &val) {
    auto *obj = reinterpret_cast<String *>(Heap::allocateNewObject(sizeof(String)));
    new(obj)String(val);
    return obj;
}

String *String::get(const char *val) {
    return get(std::string(val));
}

String::String(std::string val) : Object(BasicClasses::StringClass), val(std::move(val)) {
}
} // UltraRuby
} // Lang