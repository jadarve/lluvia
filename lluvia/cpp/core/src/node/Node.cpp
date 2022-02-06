/**
@file       Node.cpp
@brief      Node class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#include "lluvia/core/node/Node.h"

#include "lluvia/core/error.h"

#include <exception>

namespace ll {

void Node::setState(const ll::NodeState tState) {

    // state transition
    switch(m_state) {
        case ll::NodeState::Created:
        
            switch(tState) {
                case ll::NodeState::Created:
                    // do nothing
                break;
                case ll::NodeState::Init:
                    m_state = tState;
                    onInit();
                break;
            }
        break;

        case ll::NodeState::Init:

            switch(m_state) {
                case ll::NodeState::Created:
                    // TODO: what to do?
                break;
                case ll::NodeState::Init:
                    // TODO: throw exception, node already in init state
                    throw std::runtime_error("Node already in init state");
                break;
            }
        break;
    }

}

ll::NodeState Node::getState() const noexcept {
    return m_state;
}


void Node::init() {
    setState(ll::NodeState::Init);
}

} // namespace ll
