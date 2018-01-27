#ifndef LLUVIA_CORE_VISITOR_H_
#define LLUVIA_CORE_VISITOR_H_

#include <memory>
#include <string>


namespace ll {

// forward declarations
class Buffer;
class ComputeGraph;
class Memory;


class Visitor {

public:
    virtual ~Visitor() = default;

    virtual void visitComputeGraph(std::shared_ptr<ll::ComputeGraph> graph, const std::string& name = {}) {}
    virtual void visitMemory(std::shared_ptr<ll::Memory> memory, const std::string& name = {}) {}
    virtual void visitBuffer(std::shared_ptr<ll::Buffer> buffer, const std::string& name = {}) {}
};

} // namespace ll

#endif // LLUVIA_CORE_VISITOR_H_
