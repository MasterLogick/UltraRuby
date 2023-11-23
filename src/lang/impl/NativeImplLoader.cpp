#include "NativeImplLoader.h"
#include "BasicObjectImpl.h"

namespace UltraRuby {
namespace Lang {
namespace Impl {
void NativeImplLoader::loadImpl() {
    BasicObjectImpl::loadImpl();
}
} // UltraRuby
} // Lang
} // Impl