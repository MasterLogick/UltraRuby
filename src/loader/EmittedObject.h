#ifndef ULTRA_RUBY_EMITTEDOBJECT_H
#define ULTRA_RUBY_EMITTEDOBJECT_H

#include "../ir/CodeModule.h"
#include <llvm/Object/ELFObjectFile.h>

namespace UltraRuby {
namespace Loader {

class EmittedObject {
public:
    explicit EmittedObject(IR::CodeModule &codeModule);
    ~EmittedObject();

    char name[7];
private:
    int fd;
};

} // UltraRuby
} // Loader

#endif //ULTRA_RUBY_EMITTEDOBJECT_H
