#include <llvm/TargetParser/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include "EmittedObject.h"
#include <lld/Common/Driver.h>
#include <fcntl.h>

LLD_HAS_DRIVER(elf)

namespace UltraRuby {
namespace Loader {
int createTmpFile(char name[7]) {
    memcpy(name, "XXXXXX", 7);
    int d = mkstemp(name);
    return d;
}

EmittedObject::EmittedObject(IR::CodeModule &codeModule) {
    auto targetTriple = llvm::sys::getProcessTriple();

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (target == nullptr) {
        // todo throw exception
    }

    auto cpu = llvm::sys::getHostCPUName();
    auto features = "";
    llvm::TargetOptions opt;
    auto rm = std::optional<llvm::Reloc::Model>(llvm::Reloc::PIC_);
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

    codeModule.setTarget(targetMachine, targetTriple);

    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::ObjectFile;

    char oName[7];
//    int o = createTmpFile(oName);
    memcpy(oName, "obj.o", 6);
    int o = open(oName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    llvm::raw_fd_ostream dest(o, false);
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        // todo throw exception
    }

    codeModule.runPass(pass);
//    fd = createTmpFile(name);
    memcpy(name, "obj.so", 7);
    fd = open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    auto res = lld::lldMain({"ld.lld", "-z", "relro", "-shared", "--eh-frame-hdr", "-o", name, oName}, llvm::outs(),
                            llvm::errs(),
                            {{lld::Gnu, &lld::elf::link}});
    if (res.retCode != 0) {
        throw std::exception();
    }
    close(o);
}

EmittedObject::~EmittedObject() {
    close(fd);
//    remove(name);
}
} // UltraRuby
} // Loader