#include "NativeImplLoader.h"
#include "BasicObjectImpl.h"
#include "KernelImpl.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {
void NativeImplLoader::loadImpl() {
    BasicObjectImpl::loadImpl();
    KernelImpl::loadImpl();
}
} // UltraRuby
} // Lang
} // Impl