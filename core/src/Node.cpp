/**
@file       Node.cpp
@brief      Node class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/Node.h"

#include <exception>

namespace ll {

void Node::setState(const ll::NodeState tState) {

    // state transition
    switch(state) {
        case ll::NodeState::Created:
        
            switch(tState) {
                case ll::NodeState::Created:
                    // do nothing
                break;
                case ll::NodeState::Init:
                    state = tState;
                    onInit();
                break;
            }
        break;

        case ll::NodeState::Init:

            switch(state) {
                case ll::NodeState::Created:
                    // TODO: what to do?
                break;
                case ll::NodeState::Init:
                    // TODO: throw exception, node already in init state
                    throw std::runtime_error("Node alread in init state");
                break;
            }
        break;
    }

}

ll::NodeState Node::getState() const noexcept {
    return state;
}

} // namespace ll
