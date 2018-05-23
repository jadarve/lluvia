#include "lluvia/core/error.h"

namespace ll {

ErrorCategory& ErrorCategory::getInstance() noexcept {
    static ErrorCategory instance {};
    return instance;
}

} // namespace ll