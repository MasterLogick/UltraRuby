#include <llvm/Object/ELFObjectFile.h>
#include <iostream>
#include "ObjectArea.h"
#include "../lang/Object.h"
#include <map>

namespace UltraRuby {
namespace Loader {
llvm::Expected<llvm::object::SectionRef> findSection(
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
const std::map<std::string, void *> langFunctions = {
        {"_ZN9UltraRuby4Lang6Object20defineInstanceMethodEPNS0_6SymbolEiiPv", *reinterpret_cast<void **>(&Lang_Object_defineInstanceMethod)},
        {"_ZN9UltraRuby4Lang6Object5call1EPNS0_6SymbolEPS1_",                 *reinterpret_cast<void **>(&Lang_Object_call1)},
        {"_ZN9UltraRuby4Lang6Object5call0EPNS0_6SymbolE",                     *reinterpret_cast<void **>(&Lang_Object_call0)}
};

ObjectArea::EntryPoint ObjectArea::loadObject(const std::unique_ptr<llvm::object::ObjectFile> &object) {
    auto expText = findSection(object, ".text");
    if (!expText) {
        std::cout << "empty object" << std::endl;
        return nullptr;
    }
    auto textSection = *expText;
    auto textSectionSize = textSection.getSize();
    std::map<std::string, uint64_t> localSymbols = getLocalSymbols(object, textSection);

    auto content = textSection.getContents();
    if (!content) {
        std::cout << "section .text content error: " << llvm::toString(content.takeError()) << std::endl;
        return nullptr;
    }
    assert(content->size() <= textSectionSize);
    char *textRegion = static_cast<char *>(
            codeArea.allocRegion(textSectionSize
                                 + localSymbols.size() * sizeof(void *)
                                 + langFunctions.size() * (6 + sizeof(void *)),
                                 textSection.getAlignment(), MemoryArea::RWX));
    if (textRegion == nullptr) {
        return nullptr;
    }
    memcpy(textRegion, content->data(), content->size());

    void **got = reinterpret_cast<void **>(textRegion + textSectionSize);
    std::cout << "loading .text section to " << (void *) textRegion << " got table start at: " << got << std::endl;
    std::map<std::string, void **> gotMap;
    for (auto &gotEntry: localSymbols) {
        *got = textRegion + gotEntry.second;
        gotMap[gotEntry.first] = got;
        std::cout << "sym " << gotEntry.first << " loaded at: " << *got << ", got entry address: " << got << std::endl;
        got++;
    }

    char *plt = reinterpret_cast<char *>(textRegion + textSectionSize + localSymbols.size() * sizeof(void *));
    std::map<std::string, void *> pltMap;
    for (const auto &langFunction: langFunctions) {
        // generated instructions:
        // jmp near [rip]
        // dq plt_offset
        pltMap[langFunction.first] = plt;
        std::cout << "plt entry for " << langFunction.first << "(" << langFunction.second << ") generated at: "
                  << (void *) plt << std::endl;
        memcpy(plt, "\xff\x25\x00\x00\x00\x00", 6);
        plt += 6;
        *reinterpret_cast<void **>(plt) = langFunction.second;
        plt += sizeof(void *);
    }

    auto relaExp = findSection(object, ".rela.text");
    if (relaExp) {
        auto rela = *relaExp;
        for (const auto &relocation: rela.relocations()) {
            auto sym = relocation.getSymbol();
            auto symNameExp = sym->getName();
            if (!symNameExp) {
                std::cout << "obj symbol error: " << llvm::toString(symNameExp.takeError()) << std::endl;
                return nullptr;
            }
            auto symName = symNameExp->str();
            auto symSectExp = sym->getSection();
            std::string symSectName;
            if (symSectExp) {
                auto symSectNameExp = symSectExp.get()->getName();
                if (!symSectNameExp) {
                    std::cout << "sym sect symNameExp error: " << llvm::toString(symSectNameExp.takeError())
                              << std::endl;
                    return nullptr;
                }
                symSectName = symSectNameExp->str();
            }
            auto symValExp = sym->getValue();
            if (!symValExp) {
                std::cout << "sym val error: " << llvm::toString(symValExp.takeError()) << std::endl;
                return nullptr;
            }
            void *src;
            uint32_t *dest;
            if (relocation.getType() == llvm::ELF::R_X86_64_PLT32) {
                src = pltMap[symNameExp->str()];
                dest = reinterpret_cast<uint32_t *>(textRegion + relocation.getOffset());
                *dest = reinterpret_cast<char *>(src) - reinterpret_cast<char *>(dest + 1);
            } else if (relocation.getType() == llvm::ELF::R_X86_64_REX_GOTPCRELX) {
                src = gotMap[symNameExp->str()];
                dest = reinterpret_cast<uint32_t *>(textRegion + relocation.getOffset());
                *dest = reinterpret_cast<char *>(src) - reinterpret_cast<char *>(dest + 1);
            } else {
                //todo handle case
                return nullptr;
            }
            llvm::SmallVector<char, 50> d;
            relocation.getTypeName(d);
            std::string typeName(d.data(), d.size_in_bytes());
            std::cout << "resolved relocation " << typeName << " of sym " << symNameExp.get().str() << "(" << src
                      << ") from sect " << symSectName << " at " << (void *) dest << std::endl;
        }
    }
    return reinterpret_cast<EntryPoint>(*gotMap["top_required"]);
}

std::map<std::string, uint64_t> ObjectArea::getLocalSymbols(
        const std::unique_ptr<llvm::object::ObjectFile> &object, const llvm::object::SectionRef &section) {
    std::map<std::string, uint64_t> localSymbols;
    for (const auto &sym: object->symbols()) {
        auto name = sym.getName();
        if (!name) {
            continue;
        }
        auto symSectExp = sym.getSection();
        if (!symSectExp) {
            continue;
        }
        if (symSectExp.get() != section) {
            continue;
        }
        auto symValExp = sym.getValue();
        if (!symValExp) {
            continue;
        }
        localSymbols.emplace(*name, *symValExp);
    }
    return localSymbols;
}
} // UltraRuby
} // Loader