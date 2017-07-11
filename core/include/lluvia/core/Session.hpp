#ifndef LLUVIA_CORE_SESSION_HPP_
#define LLUVIA_CORE_SESSION_HPP_


#include <vector>

#include "lluvia/core/NDArray.hpp"

namespace ll {

/**
 * \brief Creates a new Session using default settings.
 */
Session createSession();


/**
 * \brief The Session class contains all the state required to run compute Graphs.
 */
class Session {

/////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////
public:
	Session();


/////////////////////////////////////////////////
// METHODS
/////////////////////////////////////////////////
public:

    NDArray createArray(const std::vector<int>& shape);
};
    
} // namespace ll

#endif // LLUVIA_CORE_SESSION_HPP_
