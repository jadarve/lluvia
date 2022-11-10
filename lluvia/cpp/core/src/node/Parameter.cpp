#include "lluvia/core/node/Parameter.h"

namespace ll {

Parameter::Parameter(ParameterType type)
    : m_type { type }
{
}

ll::ParameterType Parameter::getType() const noexcept
{
    return m_type;
}

} // namespace ll
