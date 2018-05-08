/**
@file       io.h
@brief      io related classes and methods.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_IO_H_
#define LLUVIA_CORE_IO_H_

#include <memory>
#include <string>


namespace ll {

// forward declarations
class ComputeGraph;
class Session;


/**
@brief      Reads a compute graph file and constructs it using a ll::Session.

@param[in]  filePath  The file path to the compute graph file.
@param[in]  session   The session objects used to construct the graph.

@return     A new compute graph object.

@throws     std::ifstream::failure if \ṕ filePath does not exist.
@throws     std::runtime_error if there is a problem parsing the file.
*/
std::shared_ptr<ll::ComputeGraph> readComputeGraph(const std::string& filePath, const std::shared_ptr<ll::Session>& session);


/**
@brief      Writes a compute graph to a file.

@param[in]  graph     The graph to write. It must be different than nullptr.
@param[in]  filePath  The file path.
*/
void writeComputeGraph(const std::shared_ptr<ll::ComputeGraph>& graph, const std::string& filePath);

} // namespace ll

#endif // LLUVIA_CORE_IO_H_
