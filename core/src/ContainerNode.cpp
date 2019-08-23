/**
@file       ContainerNode.cpp
@brief      ContainerNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/ContainerNode.h"

namespace ll {


ll::NodeType ContainerNode::getType() const noexcept {
    return ll::NodeType::Container;
}


// std::shared_ptr<ll::Object> ContainerNode::getPort(const std::string& name) const noexcept {

//     const auto it = m_objects.find(name);
//     return it == m_objects.end()? nullptr : it->second;
// }


// void ContainerNode::bind(const std::string& name, const std::shared_ptr<ll::Object>& obj) {

//     m_objects[name] = obj;
//     onPortBind(name, obj);
// }

} // namespace ll
