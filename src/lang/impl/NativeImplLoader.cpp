#include "NativeImplLoader.h"
#include "BasicObjectImpl.h"
#include "KernelImpl.h"
#include "ModuleImpl.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {
void NativeImplLoader::loadImpl() {
    BasicObjectImpl::loadImpl();
    KernelImpl::loadImpl();
    ModuleImpl::loadImpl();
}
} // UltraRuby
} // Lang
} // Impl