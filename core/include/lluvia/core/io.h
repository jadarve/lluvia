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
class ComputeNode;
class ComputeNodeDescriptor;
class Session;


/**
@brief      Reads a compute graph file and constructs it using a ll::Session.

@param[in]  filePath  The file path to the compute graph file.
@param[in]  session   The session objects used to construct the graph.

@return     A new compute graph object.

@throws     std::ifstream::failure if \p filePath does not exist.
@throws     std::runtime_error if there is a problem parsing the file.
*/
std::shared_ptr<ll::ComputeGraph> readComputeGraph(const std::string& filePath, const std::shared_ptr<ll::Session>& session);


/**
@brief      Writes a compute graph to a file.

@param[in]  graph     The graph to write. It must be different than nullptr.
@param[in]  filePath  The file path.
*/
void writeComputeGraph(const std::shared_ptr<ll::ComputeGraph>& graph, const std::string& filePath);


/**
@brief      Reads a compute node descriptor from a file.

Parameters \p filepath must be a valid file path to a JSON file. The JSON
document must a the same structure as that created by ll::writeComputeNodeDescriptor.

@param[in]  filePath  The file path
@param[in]  session   The session used to create the internal attributes
                      of the descriptor.

@return     a compute node descriptor.

@throws     std::ifstream::failure if \p filePath does not exist.
@throws     std::runtime_error if there is a problem parsing the file.
*/
ll::ComputeNodeDescriptor readComputeNodeDescriptor(const std::string& filePath, const std::shared_ptr<ll::Session>& session);


/**
@brief      Reads a compute node descriptor from a file.

Parameters \p filepath must be a valid file path to a JSON file. The JSON
document must a the same structure as that created by ll::writeComputeNodeDescriptor.

@param[in]  filePath  The file path
@param[in]  session   The session used to create the internal attributes
                      of the descriptor.

@return     a compute node descriptor.

@throws     std::ifstream::failure if \p filePath does not exist.
@throws     std::runtime_error if there is a problem parsing the file.
*/
ll::ComputeNodeDescriptor readComputeNodeDescriptor(const std::string& filePath, const ll::Session& session);


/**
@brief      Writes a compute node descriptor to a file.

The descriptor is written as a JSON file with the following structure:
@code
{
    "function": "main",
    "grid_x": 1,
    "grid_y": 1,
    "grid_z": 1,
    "local_x": 1,
    "local_y": 1,
    "local_z": 1,
    "parameters": [
        "Buffer",
        "ImageView"
        "SampledImageView"
    ],
    "spirv": "base 64 SPIR-V code"
}
@endcode

@param[in]  descriptor  The descriptor.
@param[in]  filePath    The file path.

@sa ll::ReadComputeNodeDescriptor   Reads a compute node descriptor from a file.

*/
void writeComputeNodeDescriptor(const ComputeNodeDescriptor& descriptor, const std::string& filePath);


/**
@brief      Reads a compute node from a file.

Parameters \p filepath must be a valid file path to a JSON file. The JSON
document must a the same structure as that created by ll::writeComputeNodeDescriptor.

@param[in]  filePath  The file path
@param[in]  session   The session used to create the compute node.

@return     a compute node.

@throws     std::ifstream::failure if \p filePath does not exist.
@throws     std::runtime_error if there is a problem parsing the file.

@sa         ll::writeComputeNode    Writes a compute node to a file.
*/
std::shared_ptr<ll::ComputeNode> readComputeNode(const std::string& filePath, const std::shared_ptr<ll::Session>& session);


/**
@brief      Reads a compute node from a file.

Parameters \p filepath must be a valid file path to a JSON file. The JSON
document must a the same structure as that created by ll::writeComputeNodeDescriptor.

@param[in]  filePath  The file path
@param[in]  session   The session used to create the compute node.

@return     a compute node.

@throws     std::ifstream::failure if \p filePath does not exist.
@throws     std::runtime_error if there is a problem parsing the file.

@sa         ll::writeComputeNode    Writes a compute node to a file.
*/
std::shared_ptr<ll::ComputeNode> readComputeNode(const std::string& filePath, const ll::Session& session);

/**
@brief      Writes a compute node to a file.

This is equivalent to:
@code
    const auto& descriptor = node->getDescriptor();
    ll::writeComputeNodeDescriptor(descriptor, filePath);
@endcode

See ll::writeComputeNodeDescriptor for the description of the JSON file
written to file.

@param[in]  node        The node.
@param[in]  filePath    The file path.

@sa ll::readComputeNode     Reads a compute node from a file.
*/
void writeComputeNode(const std::shared_ptr<ll::ComputeNode>& node, const std::string& filePath);

} // namespace ll

#endif // LLUVIA_CORE_IO_H_
