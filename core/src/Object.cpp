#include "lluvia/core/Object.h"

namespace ll {

std::string objectTypeToString(const ObjectType type) {

    switch (type) {
        case ObjectType::Buffer    : return "BUFFER";
        case ObjectType::Image     : return "IMAGE";
        case ObjectType::ImageView : return "IMAGE_VIEW";
    }
}

} // namespace ll