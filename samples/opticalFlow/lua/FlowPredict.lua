local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    ll.logd('FlowPredict', 'newDescriptor')
    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = 'FlowPredict'

    desc:addPort(ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:addParameter('iterations', 4)

    ll.logd('FlowPredict', 'newDescriptor: finish')
    return desc
end


function builder.onNodeInit(node)

    iterations = node.descriptor:getParameter('iterations')
    ll.logd('FlowPredict', 'onNodeInit: iterations:', iterations)

    dt = 1.0 / iterations
    ll.logd('FlowPredict', 'onNodeInit: dt:', dt)

    pushConstants = ll.PushConstants.new()
    pushConstants.float = dt

    in_flow = node:getPort('in_flow')

    -- Pass through the input to the output
    if iterations == 0 then
        node:bind('out_flow', in_flow)
    end

    for i = 1, iterations do
        ll.logd('FlowPredict', 'iteration: ', i)

        predictX = ll.createComputeNode('FlowPredictX')
        predictY = ll.createComputeNode('FlowPredictY')

        predictX:setParameter('dt', dt) -- FIXME
        predictX:bind('in_flow', in_flow)
        -- predictX.pushConstants = pushConstants
        predictX:init()

        predictY:setParameter('dt', dt) -- FIXME
        predictY:bind('in_flow', predictX:getPort('out_flow'))
        -- predictY.pushConstants = pushConstants
        predictY:init()

        in_flow = predictY:getPort('out_flow')

        node:bindNode(string.format('FlowPredictX_%d', i), predictX)
        node:bindNode(string.format('FlowPredictY_%d', i), predictY)
    end

    -- in_flow in the last iteration contains the output of the last cycle.
    in_flow:clear()
    node:bind('out_flow', in_flow)

    ll.logd('FlowPredict', 'onNodeInit: finish')
end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd('FlowPredict', 'onNodeRecord')

    iterations = node.descriptor:getParameter('iterations')

    for i = 1, iterations do
        ll.logd('FlowPredict', 'onNodeRecord: iteration:', i)

        predictX = node:getNode(string.format('FlowPredictX_%d', i))
        predictY = node:getNode(string.format('FlowPredictY_%d', i))

        predictX:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
        predictY:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
    end

    ll.logd('FlowPredict', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('FlowPredict', builder)
