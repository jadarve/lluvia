#include "lluvia/core/Object.h"

namespace ll {

std::string objectTypeToString(const ObjectType type) {

    switch (type) {
        case ObjectType::Buffer: return "buffer";
        case ObjectType::Image:  return "image";
    }
}

} // namespace ll