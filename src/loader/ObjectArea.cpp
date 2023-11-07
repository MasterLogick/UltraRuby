#include <llvm/Object/ELFObjectFile.h>
#include <iostream>
#include "ObjectArea.h"
#include <map>
#include <set>
#include "../lang/Array.h"
#include "../lang/Hash.h"
#include "../lang/Class.h"
#include "../lang/Module.h"
#include "../lang/Symbol.h"

namespace UltraRuby {
namespace Loader {
llvm::Expected<llvm::object::ELFSectionRef> findSection(
        const std::unique_ptr<llvm::object::ObjectFile> &object, const std::string &name) {
    for (auto &section: object->sections()) {
        auto sectName = section.getName();
        if (!sectName) {
            std::cout << "section name error: " << llvm::toString(sectName.takeError()) << std::endl;
            return {llvm::make_error<llvm::StringError>(std::error_code())};
        }
        if (!(sectName->str() != name)) {
            return {section};
        }
    }
    return {llvm::make_error<llvm::StringError>(std::error_code())};
}

auto Lang_Object_defineInstanceMethod = &Lang::Object::defineInstanceMethod;
auto Lang_Object_call0 = &Lang::Object::call0;
auto Lang_Object_call1 = &Lang::Object::call1;
auto Lang_Array_allocOnHeap = &Lang::Array::allocOnHeap;
auto Lang_Hash_allocOnHeap = &Lang::Hash::allocOnHeap;
auto Lang_Object_defineSingletonMethod = &Lang::Object::defineSingletonMethod;
auto Lang_Object_defineClassInstance = &Lang::Object::defineClassInstance;
auto Lang_Class_defineClass = &Lang::Class::defineClass;
auto Lang_Module_defineModule = &Lang::Module::defineModule;

const std::map<std::string, void *> langFunctions = {
        {"_ZN9UltraRuby4Lang5Array11allocOnHeapEiz",
                *reinterpret_cast<void **>(&Lang_Array_allocOnHeap)},
        {"_ZN9UltraRuby4Lang4Hash11allocOnHeapEiz",
                *reinterpret_cast<void **>(&Lang_Hash_allocOnHeap)},
        {"_ZN9UltraRuby4Lang6Object20defineInstanceMethodEPNS0_6SymbolEPNS0_9MethodDefE",
                *reinterpret_cast<void **>(&Lang_Object_defineInstanceMethod)},
        {"_ZN9UltraRuby4Lang6Object21defineSingletonMethodEPNS0_6SymbolEiiPv",
                *reinterpret_cast<void **>(&Lang_Object_defineSingletonMethod)},
        {"_ZN9UltraRuby4Lang6Object19defineClassInstanceEPFPS1_S2_E",
                *reinterpret_cast<void **>(&Lang_Object_defineClassInstance)},
        {"_ZN9UltraRuby4Lang5Class11defineClassEPNS0_6SymbolEPPS1_S4_PFPNS0_6ObjectES4_E",
                *reinterpret_cast<void **>(&Lang_Class_defineClass)},
        {"_ZN9UltraRuby4Lang6Module12defineModuleEPNS0_6SymbolEPPS1_S4_PFPNS0_6ObjectES4_E",
                *reinterpret_cast<void **>(&Lang_Module_defineModule)},

        {"_ZN9UltraRuby4Lang6Object5call0EPNS0_6SymbolE",
                *reinterpret_cast<void **>(&Lang_Object_call0)},
        {"_ZN9UltraRuby4Lang6Object5call1EPNS0_6SymbolEPS1_",
                *reinterpret_cast<void **>(&Lang_Object_call1)},
};

ObjectArea::EntryPoint ObjectArea::loadObject(const std::unique_ptr<llvm::object::ObjectFile> &object) {
    auto *v = reinterpret_cast<llvm::object::ELFObjectFile<llvm::object::ELF64LE> *>(object.get());
    std::vector<llvm::object::ELFSectionRef> allocatedSections;

    for (const auto &item: v->sections()) {
        llvm::object::ELFSectionRef elfSection = item;
        auto name = elfSection.getName();
        if (!name) {}
        auto b = name->str();
        if (elfSection.getFlags() & llvm::ELF::SHF_ALLOC) {
            allocatedSections.push_back(elfSection);
        }
    }

    std::sort(allocatedSections.begin(), allocatedSections.end(),
              [](llvm::object::ELFSectionRef &a, llvm::object::ELFSectionRef &b) {
                  return Log2(a.getAlignment()) >= Log2(b.getAlignment());
              });

    std::map<llvm::object::SectionRef, size_t> allocatedSectionsRelativeOffsets;
    size_t regionSize = 0;
    for (const auto &item: allocatedSections) {
        auto align = Log2(item.getAlignment());
        if (regionSize & ((1 << align) - 1)) {
            regionSize = (regionSize + (1 << align)) & ~((1 << align) - 1);
        }
        allocatedSectionsRelativeOffsets[item] = regionSize;
        regionSize += item.getSize();
    }

    auto gotRelocs = getGOTEntryNum(object);
    auto pltRelocs = getPLTEntryNum(object);
    char *region = static_cast<char *>(
            objectArea.allocRegion(regionSize
                                   + gotRelocs.size() * sizeof(void *)
                                   + pltRelocs.size() * (6 + sizeof(void *)),
                                   allocatedSections[0].getAlignment(), MemoryArea::RWX));

    for (const auto &sectOff: allocatedSectionsRelativeOffsets) {
        auto content = sectOff.first.getContents();
        if (!content) {
            std::cout << "section content error: " << llvm::toString(content.takeError()) << std::endl;
            return nullptr;
        }
        if (sectOff.first.getSize() < content->size()) {
            std::cout << "section content size exceeds section size size error: " << llvm::toString(content.takeError())
                      << std::endl;
            return nullptr;
        }
        memcpy(region + sectOff.second, content->data(), content->size());

        auto name = sectOff.first.getName();
        if (!name) {
            std::cout << "section table is corrupted: " << toString(name.takeError()) << std::endl;
            return nullptr;
        }
        std::cout << "loaded section " << name->str() << " at " << (void *) (region + sectOff.second) << "-"
                  << (void *) (region + sectOff.second + sectOff.first.getSize()) << std::endl;
    }

    void **gotTableEntryPtr = reinterpret_cast<void **>(region + regionSize);
    char *plt = reinterpret_cast<char *>(region + regionSize + gotRelocs.size() * sizeof(void *));

    std::cout << "loaded object to " << (void *) region
              << " got table starts at: " << gotTableEntryPtr
              << " plt table starts at: " << (void *) plt
              << std::endl;
    std::map<llvm::object::SymbolRef, void **> gotMap;
    for (auto &gotReloc: gotRelocs) {
        void *gotEntryVal;
        auto sect = gotReloc.getSection();
        auto val = gotReloc.getValue();
        if (!val) {
            std::cout << "relocation table is corrupted: " << toString(sect.takeError()) << std::endl;
            return nullptr;
        }
        if (!sect) {
            std::cout << "relocation table is corrupted: " << toString(sect.takeError()) << std::endl;
            return nullptr;
        }
        if (!allocatedSectionsRelativeOffsets.contains(**sect)) {
            auto name = gotReloc.getName();
            if (name && name->starts_with("sym_:")) {
                auto sym = name->substr(5);
                gotEntryVal = Lang::Symbol::get(sym.str());
            } else {
                std::cout << "relocation table is corrupted: " << toString(name.takeError()) << std::endl;
                return nullptr;
            }
        } else {
            gotEntryVal = region + allocatedSectionsRelativeOffsets[**sect] + *val;
        }
        *gotTableEntryPtr = gotEntryVal;
        gotMap[gotReloc] = gotTableEntryPtr;
        auto name = gotReloc.getName();
        if (!name) {
            std::cout << "relocation table is corrupted: " << toString(name.takeError()) << std::endl;
            return nullptr;
        }
        std::cout << "sym " << name->str() << " loaded at: " << *gotTableEntryPtr
                  << ", gotTableEntryPtr entry address: "
                  << gotTableEntryPtr << std::endl;
        gotTableEntryPtr++;
    }

    std::map<llvm::object::SymbolRef, void *> pltMap;
    for (const auto &pltReloc: pltRelocs) {
        // generated instructions:
        // jmp near [rip]
        // dq plt_offset
        pltMap[pltReloc] = plt;
        auto name = pltReloc.getName();
        if (!name) {
            std::cout << "could not resolve symbol name: " << toString(name.takeError()) << std::endl;
            return nullptr;
        }
        if (!langFunctions.contains(name->str())) {
            std::cout << name->str() << " function in plt reloc is not a lang function" << std::endl;
            return nullptr;
        }
        void *symAddress = langFunctions.at(name->str());
        std::cout << "plt entry for " << name->str() << "(" << symAddress << ") generated at: "
                  << (void *) plt << std::endl;
        memcpy(plt, "\xff\x25\x00\x00\x00\x00", 6);
        plt += 6;
        *reinterpret_cast<void **>(plt) = symAddress;
        plt += sizeof(void *);
    }

    for (const auto &relocSect: object->sections()) {
        for (const auto &relocation: relocSect.relocations()) {
            auto sym = relocation.getSymbol();
            auto relocSectionNameExp = relocSect.getName();
            if (!relocSectionNameExp) {
                std::cout << "relocation table is corrupted: " << toString(relocSectionNameExp.takeError())
                          << std::endl;
                return nullptr;
            }
            auto relocSectionName = relocSectionNameExp->str();
            if (!relocSectionName.starts_with(".rela")) {
                std::cout << "relocation table resides not in relocation section" << std::endl;
                return nullptr;
            }
            auto sect = findSection(object, relocSectionName.substr(strlen(".rela")));
            if (!sect) {
                std::cout << "relocation to nonexistent section" << std::endl;
                return nullptr;
            }
            void *src;
            void *dest = region + allocatedSectionsRelativeOffsets[*sect] + relocation.getOffset();
            uint64_t destVal;
            switch (relocation.getType()) {
                case llvm::ELF::R_X86_64_PLT32: {
                    src = pltMap[*sym];
                    destVal = *reinterpret_cast<uint32_t *>(dest) =
                            reinterpret_cast<char *>(src) - reinterpret_cast<char *>(dest) - 4;
                    break;
                }
                case llvm::ELF::R_X86_64_REX_GOTPCRELX: {
                    src = gotMap[*sym];
                    destVal = *reinterpret_cast<uint32_t *>(dest) =
                            reinterpret_cast<char *>(src) - reinterpret_cast<char *>(dest) - 4;
                    break;
                }
                case llvm::ELF::R_X86_64_64: {
                    auto sect = sym->getSection();
                    if (!sect) {
                        std::cout << "symbol table is corrupted: " << toString(sect.takeError()) << std::endl;
                        return nullptr;
                    }
                    if (!allocatedSectionsRelativeOffsets.contains(**sect)) {
                        std::cout << "entry point resides in unallocated section: " << toString(sect.takeError())
                                  << std::endl;
                        return nullptr;
                    }
                    auto val = sym->getValue();
                    if (!val) {
                        std::cout << "symbol table is corrupted: " << toString(val.takeError()) << std::endl;
                        return nullptr;
                    }
                    src = region + allocatedSectionsRelativeOffsets[**sect] + *val;
                    destVal = *reinterpret_cast<uint64_t *>(dest) =
                            reinterpret_cast<uint64_t>(src);
                    break;
                }
                case llvm::ELF::R_X86_64_PC32: {
                    src = nullptr;
                    destVal = 0;
                    break;
                }
                default: {
                    std::cout << "unimplemented relocation type " << relocation.getType() << std::endl;
                    //todo handle case
                    return nullptr;
                }
            }

            auto symNameExp = sym->getName();
            if (!symNameExp) {
                std::cout << "obj symbol error: " << llvm::toString(symNameExp.takeError()) << std::endl;
                return nullptr;
            }
            llvm::SmallVector<char, 50> d;
            relocation.getTypeName(d);
            std::string typeName(d.data(), d.size_in_bytes());
            std::cout << "resolved relocation " << typeName << " of sym " << symNameExp.get().str() << "(" << src
                      << ") at " << (void *) dest << " in " << relocSectionName.substr(strlen(".rela")) << ": "
                      << destVal << std::endl;
        }
    }

    objectArea.setRegionRights(region, regionSize, MemoryArea::RX);

    for (const auto &sym: object->symbols()) {
        auto name = sym.getName();
        if (!name) {
            std::cout << "symbol table is corrupted: " << toString(name.takeError()) << std::endl;
            return nullptr;
        }
        if (name->str() == "top_required") {
            auto sect = sym.getSection();
            if (!sect) {
                std::cout << "symbol table is corrupted: " << toString(sect.takeError()) << std::endl;
                return nullptr;
            }
            if (!allocatedSectionsRelativeOffsets.contains(**sect)) {
                std::cout << "entry point resides in unallocated section: " << toString(sect.takeError()) << std::endl;
                return nullptr;
            }
            auto val = sym.getValue();
            if (!val) {
                std::cout << "symbol table is corrupted: " << toString(val.takeError()) << std::endl;
                return nullptr;
            }
            return reinterpret_cast<EntryPoint>(region + allocatedSectionsRelativeOffsets[**sect] + *val);
        }
    }
    return nullptr;
}

std::set<llvm::object::SymbolRef> ObjectArea::getGOTEntryNum(const std::unique_ptr<llvm::object::ObjectFile> &pFile) {
    std::set<llvm::object::SymbolRef> syms;
    for (const auto &sect: pFile->sections()) {
        for (const auto &rel: sect.relocations()) {
            if (rel.getType() == llvm::ELF::R_X86_64_REX_GOTPCRELX) {
                auto b = *rel.getSymbol();
                syms.insert(b);
            }
        }
    }
    return syms;
}

std::set<llvm::object::SymbolRef> ObjectArea::getPLTEntryNum(const std::unique_ptr<llvm::object::ObjectFile> &pFile) {
    std::set<llvm::object::SymbolRef> syms;
    for (const auto &sect: pFile->sections()) {
        for (const auto &rel: sect.relocations()) {
            if (rel.getType() == llvm::ELF::R_X86_64_PLT32) {
                auto b = *rel.getSymbol();
                syms.insert(b);
            }
        }
    }
    return syms;
}
} // UltraRuby
} // Loader