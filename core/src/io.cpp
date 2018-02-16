#include "lluvia/core/io.h"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/ComputeGraph.h"
#include "lluvia/core/ComputeNode.h"
#include "lluvia/core/ComputeNodeDescriptor.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Program.h"
#include "lluvia/core/Session.h"
#include "lluvia/core/utils.h"
#include "lluvia/core/Visitor.h"

#include "json/json.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <type_traits>


using json = nlohmann::json;


namespace ll {
namespace impl {


template<typename U, typename T>
U getValueFromJson(T&& name, const json& j) {

    static_assert(std::is_convertible<T, std::string>(), "T must be a string-like type");

    const auto value = j[name];
    assert(!value.is_null());

    if (std::is_integral<U>())          assert(value.is_number_integer());
    if (std::is_floating_point<U>())    assert(value.is_number());
    if (std::is_same<U, bool>())        assert(value.is_boolean());
    if (std::is_same<U, std::string>()) assert(value.is_string());
    if (std::is_same<U, json>())        assert(value.is_object() || value.is_array());
    
    return value.template get<U>();
}


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

                    const auto pTypeString = getValueFromJson<std::string>("type", obj);
                    const auto pType = ll::stringToObjectType(pTypeString);

                    switch (pType) {
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

        const auto name           = getValueFromJson<std::string>("name", j);
        const auto pageSize       = getValueFromJson<uint64_t>("page_size", j);
        const auto memFlagsVector = getValueFromJson<json>("flags", j);

        const auto flags    = ll::vectorStringToMemoryPropertyFlags(memFlagsVector);

        auto memory = session->createMemory(flags, pageSize, false);
        graph->addMemory(name, memory);
    }


    void buildBuffer(const json& j) {

        const auto name        = getValueFromJson<std::string>("name", j);
        const auto memName     = getValueFromJson<std::string>("memory", j);;
        const auto size        = getValueFromJson<uint64_t>("size", j);;
        const auto usageVector = getValueFromJson<json>("usage", j);

        const auto usageFlags = ll::vectorStringToBufferUsageFLags(usageVector);

        // can throw std::out_of_range
        auto memory = graph->getMemory(memName);

        auto buffer = memory->createBuffer(size, usageFlags);
        graph->addObject(name, buffer);
    }


    void buildProgram(const json& j) {

        const auto name        = getValueFromJson<std::string>("name", j);
        const auto spirvString = getValueFromJson<std::string>("spirv", j);

        auto program = session->createProgram(ll::fromBase64(spirvString));
        graph->addProgram(name, program);
    }


    void buildComputeNode(const json& j) {

        const auto name         = getValueFromJson<std::string>("name", j);
        const auto functionName = getValueFromJson<std::string>("function", j);
        const auto programName  = getValueFromJson<std::string>("program", j);
        const auto globalX      = getValueFromJson<uint32_t>("global_x", j);
        const auto globalY      = getValueFromJson<uint32_t>("global_y", j);
        const auto globalZ      = getValueFromJson<uint32_t>("global_z", j);
        const auto localX       = getValueFromJson<uint32_t>("local_x", j);
        const auto localY       = getValueFromJson<uint32_t>("local_y", j);
        const auto localZ       = getValueFromJson<uint32_t>("local_z", j);
        const auto parameters   = getValueFromJson<json>("parameters", j);

        // can throw std::out_of_range
        auto program = graph->getProgram(programName);
        auto descriptor = ll::ComputeNodeDescriptor()
                            .setGlobalX(globalX)
                            .setGlobalY(globalY)
                            .setGlobalZ(globalZ)
                            .setLocalX(localX)
                            .setLocalY(localY)
                            .setLocalZ(localZ)
                            .setFunctionName(functionName)
                            .setProgram(program);


        // parameter descriptor
        for (const auto& p : parameters) {

            const auto pTypeString = getValueFromJson<std::string>("type", p);
            const auto pType       = ll::stringToObjectType(pTypeString);

            switch (pType) {
                case ll::ObjectType::Buffer:
                    descriptor.addBufferParameter();
                    break;
            }
        }

        auto computeNode = session->createComputeNode(descriptor);
        
        // parameter binding
        auto pIndex = uint32_t {0};
        for (const auto& p : parameters) {

            const auto pName       = getValueFromJson<std::string>("name", p);
            const auto pTypeString = getValueFromJson<std::string>("type", p);
            const auto pType       = ll::stringToObjectType(pTypeString);

            std::cout << pName << " : " << pTypeString << std::endl;

            // can throw std::out_of_range
            auto param = graph->getObject(pName);
            assert(param->getType() == pType);

            switch (pType) {
                case ll::ObjectType::Buffer:
                    computeNode->bind(pIndex, std::static_pointer_cast<ll::Buffer>(param));
                    break;
            }

            ++pIndex;
        }

        graph->addComputeNode(name, computeNode);
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

