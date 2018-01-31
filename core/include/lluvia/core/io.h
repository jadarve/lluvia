#ifndef LLUVIA_CORE_IO_H_
#define LLUVIA_CORE_IO_H_

#include <memory>
#include <string>


namespace ll {

// forward declarations
class ComputeGraph;
class Session;


// throws: TODO reading the file
std::shared_ptr<ll::ComputeGraph> readComputeGraph(const std::string& filePath, std::shared_ptr<ll::Session> session);

void writeComputeGraph(std::shared_ptr<ll::ComputeGraph> graph, const std::string& filePath);

} // namespace ll

#endif // LLUVIA_CORE_IO_H_
