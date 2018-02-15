#include "lluvia/core/io.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/ComputeGraph.h"
#include "lluvia/core/ComputeNode.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Program.h"
#include "lluvia/core/Session.h"
#include "lluvia/core/utils.h"
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
        obj["memories"]      = nullptr;
        obj["objects"]       = nullptr;
        obj["programs"]      = nullptr;
        obj["compute_nodes"] = nullptr;
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
        j["type"]   = ll::objectTypeToString(buffer->getType());
        j["size"]   = buffer->getSize();
        j["usage"]  = ll::bufferUsageFlagsToVectorString(buffer->getUsageFlags());

        // can throw std::out_of_range
        j["memory"] = graph->findMemoryNameForObject(name);

        obj["objects"].push_back(j);
    }


    virtual void visitProgram(std::shared_ptr<ll::Program> program, const std::string& name = {}) {

        const auto& spirv = program->getSpirV();

        auto j = json {};
        j["name"] = name;
        j["spirv"] = ll::toBase64(spirv.data(), spirv.size());

        obj["programs"].push_back(j);
    }


    virtual void visitComputeNode(std::shared_ptr<ll::ComputeNode> node, const std::string& name = {}) {

        auto j = json {};
        j["name"]       = name;
        j["program"]    = graph->findProgramNameForComputeNode(name);
        j["function"]   = node->getFunctionName();
        j["local_x"]    = node->getLocalX();
        j["local_y"]    = node->getLocalY();
        j["local_z"]    = node->getLocalZ();
        j["global_x"]   = node->getGlobalX();
        j["global_y"]   = node->getGlobalY();
        j["global_z"]   = node->getGlobalZ();
        j["parameters"] = nullptr; // placeholder

        const auto pCount = node->getParameterCount();
        for (auto i = 0u; i < pCount; ++i) {
            const auto param = node->getParameter(i);

            auto p = json {};
            p["type"] = objectTypeToString(param->getType());
            p["name"] = graph->findObjectName(param);

            j["parameters"].push_back(p);
        }

        obj["compute_nodes"].push_back(j);
    }


    std::shared_ptr<ll::ComputeGraph> graph {nullptr};
    json obj;
};


class ComputeGraphJsonReader {

public:

    std::shared_ptr<ll::ComputeGraph> build(const std::string& filePath, std::shared_ptr<ll::Session> session) {

        assert(session != nullptr);
        this->session = session;

        std::ifstream file {filePath, std::ios_base::in};

        if (file.is_open()) {

            auto j = json {};
            file >> j;

            graph = std::make_shared<ll::ComputeGraph>();

            const auto& memories = j["memories"];
            if (!memories.is_null()) {

                assert(memories.is_array());
                for (const auto& mem : memories) {
                    buildMemory(mem);
                }
            }

            const auto& objects = j["objects"];
            if (!objects.is_null()) {

                assert(objects.is_array());
                for (const auto& obj : objects) {

                    switch (ll::stringToObjectType(obj["type"].get<std::string>())) {
                        case ll::ObjectType::Buffer:
                            buildBuffer(obj);
                            break;
                        // TODO: other object types
                    }
                }
            }

            const auto& programs = j["programs"];
            if (!programs.is_null()) {

                assert(programs.is_array());
                for (const auto& prog : programs) {
                    buildProgram(prog);
                }
            }

            const auto& nodes = j["compute_nodes"];
            if (!nodes.is_null()) {

                assert(nodes.is_array());
                for (const auto& n : nodes) {
                    buildComputeNode(n);
                }
            }

            return graph;
        }

        return nullptr;
    }


    void buildMemory(const json& j) {

        const auto memFlagsVector = j["flags"];
        const auto pageSize       = j["page_size"];
        const auto name           = j["name"];

        assert(!memFlagsVector.is_null());
        assert(memFlagsVector.is_array());

        assert(!pageSize.is_null());
        assert(pageSize.is_number_integer());

        assert(!name.is_null());
        assert(name.is_string());

        const auto flags    = ll::vectorStringToMemoryPropertyFlags(memFlagsVector);

        auto memory = session->createMemory(flags, pageSize.get<uint64_t>(), false);
        graph->addMemory(name, memory);
    }


    void buildBuffer(const json& j) {

        const auto name        = j["name"];
        const auto memName     = j["memory"];
        const auto size        = j["size"];
        const auto usageVector = j["usage"];

        assert(!name.is_null());
        assert(name.is_string());

        assert(!memName.is_null());
        assert(memName.is_string());

        assert(!size.is_null());
        assert(size.is_number_integer());

        assert(!usageVector.is_null());
        assert(usageVector.is_array());

        const auto usageFlags = ll::vectorStringToBufferUsageFLags(usageVector);

        // can throw std::out_of_range
        auto memory = graph->getMemory(memName);

        auto buffer = memory->createBuffer(size, usageFlags);
        graph->addObject(name, buffer);
    }


    void buildProgram(const json& j) {

        const auto name = j["name"];
        const auto spirvString = j["spirv"];

        assert(!name.is_null());
        assert(name.is_string());

        assert(!spirvString.is_null());
        assert(spirvString.is_string());

        auto program = session->createProgram(ll::fromBase64(spirvString));
    }


    void buildComputeNode(const json& j) {
        // TODO
    }


private:
    std::shared_ptr<ll::Session> session;
    std::shared_ptr<ll::ComputeGraph> graph;

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

    auto reader = impl::ComputeGraphJsonReader {};
    return reader.build(filePath, session);
}


} // namespace ll

