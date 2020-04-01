local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

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

    local gamma = node:getParameter('gamma')
    local max_flow = node:getParameter('max_flow')
    local smooth_iterations = node:getParameter('smooth_iterations')
    local levels = node:getParameter('levels')

    ll.logd('FlowFilter', 'onNodeInit: max_flow', max_flow)

    local in_rgba = node:getPort('in_rgba')

    local RGBA2Gray = ll.createComputeNode('RGBA2Gray')
    RGBA2Gray:bind('in_rgba', in_rgba)
    RGBA2Gray:init()

    local imagePyramid = ll.createContainerNode('ImagePyramid_8ui')
    imagePyramid:setParameter('levels', levels)
    imagePyramid:bind('in_gray', RGBA2Gray:getPort('out_gray'))
    imagePyramid:init()
    
    local filterTop = ll.createContainerNode('FlowFilterSimple')
    filterTop:setParameter('gamma', gamma)
    filterTop:setParameter('max_flow', math.ceil(max_flow / (2^(levels - 1))))
    filterTop:setParameter('smooth_iterations', smooth_iterations)
    filterTop:bind('in_gray', imagePyramid:getPort('out_gray'))
    filterTop:init()

    node:bind(string.format('out_flow_%d', levels - 1), filterTop:getPort('out_flow'))
    node:bind(string.format('out_gray_%d', levels - 1), filterTop:getPort('out_gray'))

    local out_flow = filterTop:getPort('out_flow')
    local out_gray = filterTop:getPort('out_gray')

    local h = levels - 2
    while h >= 0 do

        local filterLow = ll.createContainerNode('FlowFilterDelta')
        filterLow:setParameter('gamma', gamma)
        filterLow:setParameter('max_flow', math.ceil(max_flow / (2^h)))
        filterLow:setParameter('smooth_iterations', smooth_iterations)

        local in_gray = imagePyramid:getPort(string.format('out_gray_%d', h))
        filterLow:bind('in_gray', in_gray)
        filterLow:bind('in_flow', out_flow)

        filterLow:init()
        node:bindNode(string.format('FilterLow_%d', h), filterLow)

        node:bind(string.format('out_gray_%d', h), in_gray)
        node:bind(string.format('out_flow_%d', h), filterLow:getPort('out_flow'))
        node:bind(string.format('out_delta_flow_%d', h), filterLow:getPort('out_delta_flow'))

        out_flow = filterLow:getPort('out_flow')
        out_gray = filterLow:getPort('out_gray')

        h = h - 1
    end

    node:bindNode('RGBA2Gray', RGBA2Gray)
    node:bindNode('ImagePyramid', imagePyramid)
    node:bindNode('FilterTop', filterTop)

    node:bind('out_flow', out_flow)
    node:bind('out_gray', out_gray)

    ll.logd('FlowFilter', 'onNodeInit: finish')
end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd('FlowFilter', 'onNodeRecord')

    local levels = node:getParameter('levels')

    local RGBA2Gray = node:getNode('RGBA2Gray')
    local imagePyramid = node:getNode('ImagePyramid')
    local filterTop = node:getNode('FilterTop')

    cmdBuffer:run(RGBA2Gray)
    cmdBuffer:memoryBarrier()

    imagePyramid:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    filterTop:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    local h = levels - 2
    while h >= 0 do

        local filterLow = node:getNode(string.format('FilterLow_%d', h))
        filterLow:record(cmdBuffer)
        cmdBuffer:memoryBarrier()

        h = h - 1
    end
   
    
    ll.logd('FlowFilter', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('FlowFilter', builder)
