#include "lluvia/core/ComputeGraph.h"

#include "json/json.hpp"

#include "lluvia/core/Buffer.h"
#include "lluvia/core/ComputeNode.h"
#include "lluvia/core/Memory.h"
#include "lluvia/core/Program.h"
#include "lluvia/core/Session.h"
#include "lluvia/core/Visitor.h"

#include <fstream>


using json = nlohmann::json;


namespace ll {

ComputeGraph::~ComputeGraph() {

    // destruction order
    objects.clear();
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
    assert(memory != nullptr);
    assert(!name.empty());

    memories[name] = memory;
}


std::shared_ptr<ll::Memory> ComputeGraph::getMemory(const std::string& name) const {
    return memories.at(name);
}


std::string ComputeGraph::findMemoryNameForObject(const std::string& name) const {

    // can throw std::out_of_range
    auto obj = getObject(name);

    if (obj->getType() == ll::ObjectType::Buffer) {

        auto buffer = std::dynamic_pointer_cast<ll::Buffer>(obj);

        for (const auto& it : memories) {

            if (it.second == buffer->memory) {
                return it.first;
            }
        }
    }

    // if the code reaches this point, the memory object the buffer was
    // created from is not inside the memories container.
    throw std::out_of_range(std::string{"memory not found for object: "} + name);
}


std::vector<std::string> ComputeGraph::getObjectNames() const {

    auto names = std::vector<std::string>(objects.size());

    for (const auto& v : objects) {
        names.push_back(v.first);
    }

    return names;
}


bool ComputeGraph::containsObject(const std::string& name) const noexcept{
    return objects.find(name) != objects.cend();
}


void ComputeGraph::addObject(const std::string& name, std::shared_ptr<ll::Object> object) {
    assert(object != nullptr);
    assert(!name.empty());

    objects[name] = object;
}


std::shared_ptr<ll::Object> ComputeGraph::getObject(const std::string& name) const {
    return objects.at(name);
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
    assert(program != nullptr);
    assert(!name.empty());

    programs[name] = program;
}


std::shared_ptr<ll::Program> ComputeGraph::getProgram(const std::string& name) const {
    return programs.at(name);
}


void ComputeGraph::addComputeNode(const std::string& name, std::shared_ptr<ll::ComputeNode> node) {
    assert(node != nullptr);
    assert(!name.empty());
    
    computeNodes[name] = node;
}


std::shared_ptr<ll::ComputeNode> ComputeGraph::getComputeNode(const std::string& name) const {
    return computeNodes.at(name);
}


std::string ComputeGraph::findProgramNameForComputeNode(const std::string& name) const {

    // can throw std::out_of_range
    auto node = getComputeNode(name);

    for (const auto& it : programs) {

        if (it.second == node->getProgram()) {
            return it.first;
        }
    }

    // if the code reaches this point, the program object the node was
    // created from is not inside the programs container.
    throw std::out_of_range(std::string{"program not found for node: "} + name);
}


std::string ComputeGraph::findObjectName(std::shared_ptr<ll::Object> param) {

    for (const auto& it : objects) {
        if (it.second == param) {
            return it.first;
        }
    }

    throw std::out_of_range(std::string{"object name not found: "});
}


void ComputeGraph::accept(ll::Visitor* visitor) {
    assert(visitor != nullptr);

    for (const auto& it : memories) {
        visitor->visitMemory(it.second, it.first);
    }

    for (const auto& it : objects) {

        auto& obj = it.second;

        if (obj->getType() == ll::ObjectType::Buffer) {
            visitor->visitBuffer(std::dynamic_pointer_cast<ll::Buffer>(obj), it.first);    
        }
        
    }

    for (const auto& it : programs) {
        visitor->visitProgram(it.second, it.first);
    }

    for (const auto& it : computeNodes) {
        visitor->visitComputeNode(it.second, it.first);
    }
}

} // namespace ll
