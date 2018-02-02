#include "lluvia/core/io.h"

#include "lluvia/core/ComputeGraph.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Buffer.h"
#include "lluvia/core/utils.h"
#include "lluvia/core/Session.h"
#include "lluvia/core/Visitor.h"


#include "json/json.hpp"


#include <iostream>
#include <fstream>


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
        j["name"]   = name;
        j["size"]   = buffer->getSize();
        j["usage"]  = ll::bufferUsageFlagsToVectorString(buffer->getUsageFlags());

        // can throw std::out_of_range
        j["memory"] = graph->getMemoryNameForBuffer(name);

        obj["buffers"].push_back(j);
    }


    std::shared_ptr<ll::ComputeGraph> graph {nullptr};
    json obj;
};

} // namespace impl


void writeComputeGraph(std::shared_ptr<ll::ComputeGraph> graph, const std::string& filePath) {

    auto visitor = impl::ComputeGraphFileWriterImpl {};

    // while visiting the graph, std::out_of_range can be thrown if
    // the memory of a buffer object is not stored in the graph
    visitor.visitComputeGraph(graph);
    graph->accept(&visitor);

    // TODO create ofstream and write pimpl->obj
    std::cout << std::setw(4) << visitor.obj << std::endl;
}


std::shared_ptr<ll::ComputeGraph> readComputeGraph(const std::string& filePath, std::shared_ptr<ll::Session> session) {

    assert(session != nullptr);

    std::ifstream file {filePath, std::ios_base::in};

    try {

        if (file.is_open()) {

            auto j = json {};
            file >> j;

            auto graph = std::make_shared<ll::ComputeGraph>();

            // memories
            for (const auto& mem : j["memories"]) {

                const auto memFlagsVector = mem["flags"];
                const auto pageSize       = mem["page_size"];
                const auto name           = mem["name"];

                assert (memFlagsVector != nullptr);
                assert (memFlagsVector.is_array());

                assert (pageSize != nullptr);
                assert (pageSize.is_number_integer());

                assert (name != nullptr);
                assert (name.is_string());

                const auto flags    = ll::vectorStringToMemoryPropertyFlags(memFlagsVector);

                // TODO: need to include exact match flags
                auto memory = session->createMemory(flags, pageSize.get<uint64_t>(), false);
                graph->addMemory(name, memory);
            }

            // buffers
            for (const auto& buf : j["buffers"]) {

                const auto name        = buf["name"];
                const auto memName     = buf["memory"];
                const auto size        = buf["size"];
                const auto usageVector = buf["usage"];

                assert (name != nullptr);
                assert (name.is_string());

                assert (memName != nullptr);
                assert (memName.is_string());

                assert (size != nullptr);
                assert (size.is_number_integer());

                assert (usageVector != nullptr);
                assert (usageVector.is_array());

                const auto usageFlags = ll::vectorStringToBufferUsageFLags(usageVector);

                // can throw std::out_of_range
                auto memory = graph->getMemory(memName);

                auto buffer = memory->createBuffer(size, usageFlags);
                graph->addBuffer(name, buffer);

            }

            return graph;
            // std::cout << std::setw(4) << j << std::endl;
        }

    }
    catch (const json::exception& e) {

        // json::exceptions should not be thrown out of this function
        std::cerr << "json exception" << std::endl;
    }
    catch (const std::out_of_range& e) {

        // some element in the graph was not found.
        std::cerr << "out_of_range exception" << std::endl;
    }

    
    return nullptr;
}


} // namespace ll

