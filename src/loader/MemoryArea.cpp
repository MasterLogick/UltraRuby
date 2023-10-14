#include "MemoryArea.h"
#include <sys/mman.h>
#include <unistd.h>

namespace UltraRuby {
namespace Loader {
void *MemoryArea::allocRegion(size_t size, llvm::Align align, int rights) {
    auto prot = PROT_NONE;
    if (rights & R) {
        prot |= PROT_READ;
    }
    if (rights & W) {
        prot |= PROT_WRITE;
    }
    if (rights & X) {
        prot |= PROT_EXEC;
    }
    auto x = &MemoryArea::allocRegion;
    void **t = reinterpret_cast<void **>(&x);
    auto retVal = mmap(*t, size, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (retVal == MAP_FAILED) {
        perror(strerror(errno));
        return nullptr;
    }
    return retVal;
}
} // UltraRuby
} // Loader