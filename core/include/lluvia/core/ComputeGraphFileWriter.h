#ifndef LLUVIA_CORE_COMPUTE_GRAPH_FILE_WRITER_H_
#define LLUVIA_CORE_COMPUTE_GRAPH_FILE_WRITER_H_

#include <memory>
#include <string>


namespace ll {

class Buffer;
class ComputeGraph;
class Memory;


namespace impl {

class ComputeGraphFileWriterImpl;

} // namespace impl


class ComputeGraphFileWriter {

public:
    ComputeGraphFileWriter();
    ComputeGraphFileWriter(const ComputeGraphFileWriter&) = default; // not copiable
    ComputeGraphFileWriter(ComputeGraphFileWriter&&)      = default;

    virtual ~ComputeGraphFileWriter();

    ComputeGraphFileWriter& operator = (const ComputeGraphFileWriter&) = default; // not copiable
    ComputeGraphFileWriter& operator = (ComputeGraphFileWriter&&)      = default;

    void write(std::shared_ptr<ll::ComputeGraph> graph, const std::string& filePath);

private:
    impl::ComputeGraphFileWriterImpl* pimpl {nullptr};
};

} // namespace ll

#endif // LLUVIA_CORE_COMPUTE_GRAPH_FILE_WRITER_H_
