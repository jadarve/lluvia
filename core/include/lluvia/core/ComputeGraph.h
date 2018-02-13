#ifndef LLUVIA_CORE_COMPUTE_GRAPH_H_
#define LLUVIA_CORE_COMPUTE_GRAPH_H_

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

namespace ll {

// forward declarations
class Buffer;
class ComputeNode;
class Memory;
class Object;
class Program;
class Session;
class Visitor;

/**
 * \brief Compute graph.
 */
class ComputeGraph {

public:
    ComputeGraph()                                      = default;
    ComputeGraph(const ComputeGraph& node)              = delete;   // not copiable
    ComputeGraph(ComputeGraph&& node)                   = default;

    ~ComputeGraph();

    ComputeGraph& operator = (const ComputeGraph& node) = delete;   // not copiable
    ComputeGraph& operator = (ComputeGraph&& node)      = default;

    std::vector<std::string> getMemoryNames() const;
    bool containsMemory(const std::string& name) const noexcept;
    void addMemory(const std::string& name, std::shared_ptr<ll::Memory> memory);
    std::shared_ptr<ll::Memory> getMemory(const std::string& name) const;
    std::string findMemoryNameForBuffer(const std::string& name) const;

    std::vector<std::string> getBufferNames() const;
    bool containsBuffer(const std::string& name) const noexcept;
    void addBuffer(const std::string& name, std::shared_ptr<ll::Buffer> buffer);
    std::shared_ptr<ll::Buffer> getBuffer(const std::string& name) const;

    std::vector<std::string> getProgramNames() const;
    bool containsProgram(const std::string& name) const noexcept;
    void addProgram(const std::string& name, std::shared_ptr<ll::Program> program);
    std::shared_ptr<ll::Program> getProgram(const std::string& name) const;

    void addComputeNode(const std::string& name, std::shared_ptr<ll::ComputeNode> node);
    std::shared_ptr<ll::ComputeNode> getComputeNode(const std::string& name) const;
    std::string findProgramNameForComputeNode(const std::string& name) const;

    std::string findObjectName(std::shared_ptr<ll::Object> param);

    void accept(ll::Visitor* visitor);

private:
    std::unordered_map<std::string, std::shared_ptr<ll::Memory>>      memories;
    std::unordered_map<std::string, std::shared_ptr<ll::Buffer>>      buffers;
    std::unordered_map<std::string, std::shared_ptr<ll::Program>>     programs;
    std::unordered_map<std::string, std::shared_ptr<ll::ComputeNode>> computeNodes;


    // TODO: command buffer with all the compute nodes recorded.
    
};


} // namespace ll

#endif /* LLUVIA_CORE_COMPUTE_GRAPH_H_ */
