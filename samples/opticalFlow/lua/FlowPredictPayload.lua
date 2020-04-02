local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = 'FlowPredictPayload'

    desc:addPort(ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'in_vector', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(3, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(4, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(5, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('max_flow', 4)

    return desc
end


function builder.onNodeInit(node)

    local max_flow = math.ceil(node.descriptor:getParameter('max_flow'))
    ll.logd('FlowPredictPayload', 'onNodeInit: max_flow:', max_flow)

    -- max_flow could be any floating number. The round-up value
    -- is the number of iterations to propagate.
    local N = math.ceil(max_flow)

    local dt = 1.0 / N
    ll.logd('FlowPredictPayload', 'onNodeInit: dt:', dt)

    local pushConstants = ll.PushConstants.new()
    pushConstants.float = dt

    local in_flow = node:getPort('in_flow')
    local in_gray = node:getPort('in_gray')
    local in_vector = node:getPort('in_vector')

    -- Pass through the input to the output
    if N == 0 then
        node:bind('out_flow', in_flow)
    end

    for i = 1, N do
        ll.logd('FlowPredictPayload', 'iteration: ', i)

        local predictX = ll.createComputeNode('FlowPredictPayloadX')
        local predictY = ll.createComputeNode('FlowPredictPayloadY')

        predictX:setParameter('dt', dt)
        predictX:bind('in_flow', in_flow)
        predictX:bind('in_gray', in_gray)
        predictX:bind('in_vector', in_vector)
        predictX:init()

        predictY:setParameter('dt', dt)
        predictY:bind('in_flow', predictX:getPort('out_flow'))
        predictY:bind('in_gray', predictX:getPort('out_gray'))
        predictY:bind('in_vector', predictX:getPort('out_vector'))
        predictY:init()

        in_flow = predictY:getPort('out_flow')
        in_gray = predictY:getPort('out_gray')
        in_vector = predictY:getPort('out_vector')

        node:bindNode(string.format('FlowPredictPayloadX_%d', i), predictX)
        node:bindNode(string.format('FlowPredictPayloadY_%d', i), predictY)
    end

    -- in_flow in the last iteration contains the output of the last cycle.
    in_flow:clear()
    in_gray:clear()
    in_vector:clear()
    
    node:bind('out_flow', in_flow)
    node:bind('out_gray', in_gray)
    node:bind('out_vector', in_vector)

    ll.logd('FlowPredictPayload', 'onNodeInit: finish')
end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd('FlowPredictPayload', 'onNodeRecord')

    local max_flow = node.descriptor:getParameter('max_flow')
    local N = math.ceil(max_flow)

    for i = 1, N do
        ll.logd('FlowPredictPayload', 'onNodeRecord: iteration:', i)

        local predictX = node:getNode(string.format('FlowPredictPayloadX_%d', i))
        local predictY = node:getNode(string.format('FlowPredictPayloadY_%d', i))

        predictX:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
        predictY:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
    end

    ll.logd('FlowPredictPayload', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('FlowPredictPayload', builder)
