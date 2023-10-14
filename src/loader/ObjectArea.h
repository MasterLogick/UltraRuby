#ifndef ULTRA_RUBY_OBJECTAREA_H
#define ULTRA_RUBY_OBJECTAREA_H

#include <memory>
#include <llvm/Object/ObjectFile.h>
#include "MemoryArea.h"
#include "../lang/Object.h"

namespace UltraRuby {
namespace Loader {
class ObjectArea {
public:
    using EntryPoint = Lang::Object *(*)(Lang::Object *);

    EntryPoint loadObject(const std::unique_ptr<llvm::object::ObjectFile> &object);

private:
    MemoryArea codeArea;
    MemoryArea gdt;

    std::map<std::string, uint64_t>
    getLocalSymbols(const std::unique_ptr<llvm::object::ObjectFile> &object, const llvm::object::SectionRef &section);
};
} // UltraRuby
} // Loader

#endif //ULTRA_RUBY_OBJECTAREA_H
