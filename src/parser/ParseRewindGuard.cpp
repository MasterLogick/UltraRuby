#include "ParseRewindGuard.h"

namespace UltraRuby {
namespace Parser {
ParseRewindGuard::ParseRewindGuard(Parser *parser) : parser(parser), rewind(parser->queue->getPtr()) {}

ParseRewindGuard::~ParseRewindGuard() {
    if (rewind != -1)
        parser->rewindTo(rewind);
}

void ParseRewindGuard::release() {
    rewind = -1;
}
} // UltraRuby
} // Parser