/**
@file       Node.cpp
@brief      Node class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/new/Node.h"

#include <exception>

namespace ll {
namespace news {

void Node::setState(const ll::news::NodeState tState) {

    // state transition
    switch(state) {
        case ll::news::NodeState::Created:
        
            switch(tState) {
                case ll::news::NodeState::Created:
                    // do nothing
                break;
                case ll::news::NodeState::Init:
                    state = tState;
                    onInit();
                break;
            }
        break;

        case ll::news::NodeState::Init:

            switch(state) {
                case ll::news::NodeState::Created:
                    // TODO: what to do?
                break;
                case ll::news::NodeState::Init:
                    // TODO: throw exception, node already in init state
                    throw std::runtime_error("Node alread in init state");
                break;
            }
        break;
    }

}

ll::news::NodeState Node::getState() const noexcept {
    return state;
}

} // namespace news
} // namespace ll
