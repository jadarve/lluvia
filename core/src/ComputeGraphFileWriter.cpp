#include "lluvia/core/ComputeGraphFileWriter.h"

#include "lluvia/core/ComputeGraph.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Buffer.h"
#include "lluvia/core/utils.h"
#include "lluvia/core/Visitor.h"


#include "json/json.hpp"


#include <iostream>


using json = nlohmann::json;


namespace ll {

namespace impl {


class ComputeGraphFileWriterImpl : public ll::Visitor {

public:
    void visitComputeGraph(std::shared_ptr<ll::ComputeGraph> graph, const std::string& name = {}) {

        this->graph = graph;
        
        // placeholders
        obj["memories"] = nullptr;
        obj["buffers"]  = nullptr;
    }


    void visitMemory(std::shared_ptr<ll::Memory> memory, const std::string& name = {}) {

        auto j = json {};
        j["name"]      = name;
        j["flags"]     = ll::memoryPropertyFlagsToVectorString(memory->getMemoryPropertyFlags());
        j["page_size"] = memory->getPageSize();

        obj["memories"].push_back(j);
    }


    void visitBuffer(std::shared_ptr<ll::Buffer> buffer, const std::string& name = {}) {

        auto j = json {};
        j["name"]  = name;
        j["size"]  = buffer->getSize();
        j["usage"] = ll::bufferUsageFlagsToVectorString(buffer->getUsageFlags());

        // TODO
        j["memory"] = "memory_name";

        obj["buffers"].push_back(j);
    }


    std::shared_ptr<ll::ComputeGraph> graph {nullptr};
    json obj;
};

} // namespace impl



///////////////////////////////////////////////////////////
// ComputeGraphFileWriter
///////////////////////////////////////////////////////////


void ComputeGraphFileWriter::write(std::shared_ptr<ll::ComputeGraph> graph, const std::string& filePath) {

    auto visitor = impl::ComputeGraphFileWriterImpl {};
    visitor.visitComputeGraph(graph);
    graph->accept(&visitor);

    // TODO create ofstream and write pimpl->obj
    std::cout << std::setw(4) << visitor.obj << std::endl;
}


} // namespace ll

