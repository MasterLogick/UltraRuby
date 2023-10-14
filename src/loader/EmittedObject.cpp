#include <llvm/TargetParser/Host.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include "EmittedObject.h"
#include <sys/mman.h>
#include <fcntl.h>

namespace UltraRuby {
namespace Loader {
EmittedObject::EmittedObject(IR::CodeGenerator &codeGenerator) {
    auto targetTriple = llvm::sys::getProcessTriple();

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (target == nullptr) {
        // todo throw exception
    }

    auto cpu = llvm::sys::getHostCPUName();
    auto features = "";
    llvm::TargetOptions opt;
    auto rm = std::optional<llvm::Reloc::Model>(llvm::Reloc::Static);
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

    codeGenerator.setTarget(targetMachine, targetTriple);

    llvm::legacy::PassManager pass;
    auto fileType = llvm::CodeGenFileType::ObjectFile;

    // int fd = open("output.o", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    int fd = memfd_create("", 0);
    llvm::raw_fd_ostream dest(fd, false);
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        // todo throw exception
    }

    codeGenerator.runPass(pass);
    len = lseek64(fd, 0, SEEK_END);
    data = static_cast<char *>(mmap(nullptr, len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0));
}

llvm::Expected<std::unique_ptr<llvm::object::ObjectFile>> EmittedObject::createELFObject() {
    return llvm::object::ObjectFile::createELFObjectFile(llvm::MemoryBufferRef(llvm::StringRef(data, len), ""));
}
} // UltraRuby
} // Loader