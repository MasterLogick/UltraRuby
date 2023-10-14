#ifndef ULTRA_RUBY_EMITTEDOBJECT_H
#define ULTRA_RUBY_EMITTEDOBJECT_H

#include "../ir/CodeGenerator.h"
#include <llvm/Object/ELFObjectFile.h>

namespace UltraRuby {
namespace Loader {

class EmittedObject {
public:
    explicit EmittedObject(IR::CodeGenerator &codeGenerator);

    llvm::Expected<std::unique_ptr<llvm::object::ObjectFile>> createELFObject();

private:
    size_t len;
    char *data;
};

} // UltraRuby
} // Loader

#endif //ULTRA_RUBY_EMITTEDOBJECT_H
