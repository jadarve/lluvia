#ifndef LLUVIA_CORE_COMPUTE_GRAPH_FILE_WRITER_H_
#define LLUVIA_CORE_COMPUTE_GRAPH_FILE_WRITER_H_

#include <memory>
#include <string>


namespace ll {

// forward declarations
class ComputeGraph;


class ComputeGraphFileWriter {

public:

    // throws: std::out_of_range if any element in the graph is not found
    // throws: TODO writing the file
    void write(std::shared_ptr<ll::ComputeGraph> graph, const std::string& filePath);
};

} // namespace ll

#endif // LLUVIA_CORE_COMPUTE_GRAPH_FILE_WRITER_H_
