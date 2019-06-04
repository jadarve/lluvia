/**
@file       ComputeNode.h
@brief      ComputeNode class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_NEW_COMPUTE_NODE_H_
#define LLUVIA_CORE_NEW_COMPUTE_NODE_H_

#include "lluvia/core/new/Node.h"


namespace ll {
namespace news {

class ContainerNode : public news::Node {

public:
    ll::news::NodeType getType() const noexcept override;

protected:
    void onInit() override;

private:
    
};

} // namespace news
} // namespace ll

#endif // LLUVIA_CORE_NEW_COMPUTE_NODE_H_
