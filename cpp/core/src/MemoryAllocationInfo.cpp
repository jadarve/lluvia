#include "lluvia/core/MemoryAllocationInfo.h"

std::ostream& operator<< (std::ostream &out, const ll::MemoryAllocationInfo& allocInfo) {
    out << "["  << allocInfo.offset;
    out << ", " << allocInfo.size;
    out << ", " << allocInfo.leftPadding;
    out << ", " << allocInfo.page << "]";
    return out;
}
