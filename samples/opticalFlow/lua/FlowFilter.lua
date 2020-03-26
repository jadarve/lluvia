local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    ll.logd('FlowFilter', 'newDescriptor')
    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = 'FlowFilter'

    desc:addPort(ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:setParameter('max_flow', 4)
    desc:setParameter('levels', 1)
    desc:setParameter('gamma', 0.01)
    desc:setParameter('smooth_iterations', 1)

    return desc
end


function builder.onNodeInit(node)

    ll.logd('FlowFilter', 'onNodeInit')

    gamma = node:getParameter('gamma')
    max_flow = node:getParameter('max_flow')
    smooth_iterations = node:getParameter('smooth_iterations')
    levels = node:getParameter('levels')

    ll.logd('FlowFilter', 'onNodeInit: max_flow', max_flow)

    in_rgba = node:getPort('in_rgba')

    RGBA2Gray = ll.createComputeNode('RGBA2Gray')
    RGBA2Gray:bind('in_rgba', in_rgba)
    RGBA2Gray:init()

    imagePyramid = ll.createContainerNode('ImagePyramid_8ui')
    imagePyramid:setParameter('levels', levels)
    imagePyramid:bind('in_gray', RGBA2Gray:getPort('out_gray'))
    imagePyramid:init()
    
    filterTop = ll.createContainerNode('FlowFilterSimple')
    filterTop:bind('in_gray', imagePyramid:getPort('out_gray'))
    filterTop:init()

    node:bindNode('RGBA2Gray', RGBA2Gray)
    node:bindNode('ImagePyramid', imagePyramid)
    node:bindNode('FilterTop', filterTop)

    node:bind('out_flow', filterTop:getPort('out_flow'))
    node:bind('out_gray', filterTop:getPort('out_gray'))

    ll.logd('FlowFilter', 'onNodeInit: finish')

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd('FlowFilter', 'onNodeRecord')

    RGBA2Gray = node:getNode('RGBA2Gray')
    imagePyramid = node:getNode('ImagePyramid')
    filterTop = node:getNode('FilterTop')

    cmdBuffer:run(RGBA2Gray)
    cmdBuffer:memoryBarrier()

    -- cmdBuffer:run(imagePyramid)
    imagePyramid:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    filterTop:record(cmdBuffer)
    cmdBuffer:memoryBarrier()
   
    
    ll.logd('FlowFilter', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('FlowFilter', builder)
