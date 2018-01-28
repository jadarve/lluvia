#include "lluvia/core/ComputeGraph.h"

#include "json/json.hpp"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Program.h"
#include "lluvia/core/Session.h"
#include "lluvia/core/Visitor.h"

#include <fstream>


using json = nlohmann::json;


namespace ll {

ComputeGraph::~ComputeGraph() {

    // destruction order
    buffers.clear();
    memories.clear();
}

std::vector<std::string> ComputeGraph::getMemoryNames() const {

    auto names = std::vector<std::string>(memories.size());

    for (const auto& v : memories) {
        names.push_back(v.first);
    }

    return names;
}


bool ComputeGraph::containsMemory(const std::string& name) const noexcept {
    return memories.find(name) != memories.cend();
}


void ComputeGraph::addMemory(const std::string& name, std::shared_ptr<ll::Memory> memory) {
    memories[name] = memory;
}


std::shared_ptr<ll::Memory> ComputeGraph::getMemory(const std::string& name) const {
    return memories.at(name);
}


std::vector<std::string> ComputeGraph::getBufferNames() const {

    auto names = std::vector<std::string>(buffers.size());

    for (const auto& v : buffers) {
        names.push_back(v.first);
    }

    return names;
}


bool ComputeGraph::containsBuffer(const std::string& name) const noexcept{
    return buffers.find(name) != buffers.cend();
}


void ComputeGraph::addBuffer(const std::string& name, std::shared_ptr<ll::Buffer> buffer) {
    buffers[name] = buffer;
}


std::shared_ptr<ll::Buffer> ComputeGraph::getBuffer(const std::string& name) const {
    return buffers.at(name);
}


std::string ComputeGraph::getMemoryNameForBuffer(const std::string& name) const {

    // can throw std::out_of_range
    auto buffer = getBuffer(name);

    for (const auto& it : memories) {

        if (it.second.get() == buffer->memory) {
            return it.first;
        }
    }

    // if the code reaches this point, the memory object the buffer was
    // created from is not inside the memories container.
    throw std::out_of_range(std::string{"memory not found for buffer: "} + name);
}


std::vector<std::string> ComputeGraph::getProgramNames() const {

    auto names = std::vector<std::string>(programs.size());

    for (const auto& v : programs) {
        names.push_back(v.first);
    }

    return names;
}


bool ComputeGraph::containsProgram(const std::string& name) const noexcept {
    return programs.find(name) != programs.cend();
}


void ComputeGraph::addProgram(const std::string& name, std::shared_ptr<ll::Program> program) {
    programs[name] = program;
}


std::shared_ptr<ll::Program> ComputeGraph::getProgram(const std::string& name) const {
    return programs.at(name);
}


void ComputeGraph::accept(ll::Visitor* visitor) {
    assert(visitor != nullptr);

    for (const auto& it : memories) {
        visitor->visitMemory(it.second, it.first);
    }

    for (const auto& it : buffers) {
        visitor->visitBuffer(it.second, it.first);
    }
}

} // namespace ll
