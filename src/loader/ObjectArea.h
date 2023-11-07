#ifndef ULTRA_RUBY_OBJECTAREA_H
#define ULTRA_RUBY_OBJECTAREA_H

#include <memory>
#include <llvm/Object/ObjectFile.h>
#include <set>
#include "MemoryArea.h"
#include "../lang/Object.h"

namespace UltraRuby {
namespace Loader {
class ObjectArea {
public:
    using EntryPoint = Lang::Object *(*)(Lang::Object *);

    EntryPoint loadObject(const std::unique_ptr<llvm::object::ObjectFile> &object);

private:
    MemoryArea objectArea;

    static std::set<llvm::object::SymbolRef> getGOTEntryNum(const std::unique_ptr<llvm::object::ObjectFile> &pFile);

    static std::set<llvm::object::SymbolRef> getPLTEntryNum(const std::unique_ptr<llvm::object::ObjectFile> &pFile);
};
} // UltraRuby
} // Loader

#endif //ULTRA_RUBY_OBJECTAREA_H
