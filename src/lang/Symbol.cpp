#include "Symbol.h"

namespace UltraRuby {
namespace Lang {
Symbol *Symbol::get(std::string str) {
    return nullptr;
}

Symbol *Symbol::get(const char *str) {
    return get(std::string(str));
}
} // UltraRuby
} // Lang