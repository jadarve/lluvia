/**
@file       Visitor.h
@brief      Visitor class.
@copyright  2018, Juan David Adarve Bermudez. See AUTHORS for more details.
            Distributed under the Apache-2 license, see LICENSE for more details.
*/

#ifndef LLUVIA_CORE_VISITOR_H_
#define LLUVIA_CORE_VISITOR_H_

#include <memory>
#include <string>


namespace ll {

// forward declarations
class Buffer;
class ComputeGraph;
class ComputeNode;
class Image;
class ImageView;
class Memory;
class Program;


/**
@brief      Interface to be notified of the composition of a compute graph.

In particular, visitors are implemented for writing a file representation
of ll::ComputeGraph objects.
*/
class Visitor {

public:
    virtual ~Visitor() = default;


    /**
    @brief      Visits a ll::ComputeGraph.
    
    @param[in]  graph  The graph
    @param[in]  name   The graph's name. Defaults to empty string.
    */
    virtual void visitComputeGraph(const std::shared_ptr<ll::ComputeGraph>& graph, const std::string& name = {}) {}


    /**
    @brief      Visits a ll::Memory.
    
    @param[in]  memory  The memory.
    @param[in]  name    The memory's name.  Defaults to empty string.
    */
    virtual void visitMemory(const std::shared_ptr<ll::Memory>& memory, const std::string& name = {}) {}


    /**
    @brief      Visits a ll::Buffer.
    
    @param[in]  buffer  The buffer.
    @param[in]  name    The buffer's name. Defaults to empty string.
    */
    virtual void visitBuffer(const std::shared_ptr<ll::Buffer>& buffer, const std::string& name = {}) {}


    /**
    @brief      Visits a ll::Image.
    
    @param[in]  image  The image.
    @param[in]  name   The image's name.
    */
    virtual void visitImage(const std::shared_ptr<ll::Image>& image, const std::string& name = {}) {}


    /**
    @brief      Visits a ll::ImageView.
    
    @param[in]  imageView  The image view
    @param[in]  name       The image view's name. Defaults to empty string.
    */
    virtual void visitImageView(const std::shared_ptr<ll::ImageView>& imageView, const std::string& name = {}) {}


    /**
    @brief      Visits a ll::Program.
    
    @param[in]  program  The program
    @param[in]  name     The program's name. Defaults to empty string.
    */
    virtual void visitProgram(const std::shared_ptr<ll::Program>& program, const std::string& name = {}) {}


    /**
    @brief      Visits a ll::ComputeNode.
    
    @param[in]  node  The node
    @param[in]  name  The node's name. Defaults to empty string.
    */
    virtual void visitComputeNode(const std::shared_ptr<ll::ComputeNode>& node, const std::string& name = {}) {}
};

} // namespace ll

#endif // LLUVIA_CORE_VISITOR_H_
