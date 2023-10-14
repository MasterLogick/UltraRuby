#ifndef ULTRA_RUBY_MEMORYAREA_H
#define ULTRA_RUBY_MEMORYAREA_H

#include <cstddef>
#include <llvm/Support/Alignment.h>

namespace UltraRuby {
namespace Loader {

class MemoryArea {
public:
    static constexpr int R = 1;
    static constexpr int W = 2;
    static constexpr int X = 4;
    static constexpr int RW = R | W;
    static constexpr int RWX = R | W | X;
    static constexpr int RX = R | X;

    void *allocRegion(size_t size, llvm::Align align, int rights);

    void setRegionRights(void *ptr, size_t size, int rights);

private:
    struct TextBlock {
        size_t filled;
        size_t blockSize;
        TextBlock *next;
        TextBlock *prev;
    };
    static_assert(sizeof(TextBlock) == 32);
    TextBlock *currentBlock = nullptr;
};

} // UltraRuby
} // Loader

#endif //ULTRA_RUBY_MEMORYAREA_H
