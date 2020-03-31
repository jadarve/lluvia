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
    filterTop:setParameter('gamma', gamma)
    filterTop:setParameter('max_flow', max_flow)
    filterTop:setParameter('smooth_iterations', smooth_iterations)
    filterTop:bind('in_gray', imagePyramid:getPort('out_gray'))
    filterTop:init()

    node:bind(string.format('out_flow_%d', levels - 1), filterTop:getPort('out_flow'))
    node:bind(string.format('out_gray_%d', levels - 1), filterTop:getPort('out_gray'))

    in_flow = filterTop:getPort('out_flow')

    h = levels - 2
    while h >= 0 do

        filterLow = ll.createContainerNode('FlowFilterDelta')
        filterLow:setParameter('gamma', gamma)
        filterLow:setParameter('max_flow', max_flow)
        filterLow:setParameter('smooth_iterations', smooth_iterations)

        in_gray = imagePyramid:getPort(string.format('out_gray_%d', h))
        filterLow:bind('in_gray', in_gray)
        filterLow:bind('in_flow', in_flow)

        filterLow:init()
        node:bindNode(string.format('FilterLow_%d', h), filterLow)

        node:bind(string.format('out_gray_%d', h), in_gray)
        node:bind(string.format('out_flow_%d', h), filterLow:getPort('out_flow'))
        node:bind(string.format('out_delta_flow_%d', h), filterLow:getPort('out_delta_flow'))

        in_flow = filterLow:getPort('out_flow')

        h = h - 1
    end

    node:bindNode('RGBA2Gray', RGBA2Gray)
    node:bindNode('ImagePyramid', imagePyramid)
    node:bindNode('FilterTop', filterTop)

    node:bind('out_flow', filterLow:getPort('out_flow'))
    node:bind('out_gray', filterLow:getPort('out_gray'))

    ll.logd('FlowFilter', 'onNodeInit: finish')

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd('FlowFilter', 'onNodeRecord')

    levels = node:getParameter('levels')

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

    h = levels - 2
    while h >= 0 do

        filterLow = node:getNode(string.format('FilterLow_%d', h))
        filterLow:record(cmdBuffer)
        cmdBuffer:memoryBarrier()

        h = h - 1
    end
   
    
    ll.logd('FlowFilter', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('FlowFilter', builder)
