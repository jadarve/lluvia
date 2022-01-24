local builder = ll.class(ll.ContainerNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/FlowPredict'
builder.doc = [[
Computes a forward prediction of an optical flow field.

The prediction is computed as a series of X and Y predictions using
the FlowPredictX and FlowPredictY compute nodes.

Based on the value of max_flow parameter, the time step dt is calculated as:

dt = 1.0 / ceil(max_flow)

Parameters
----------
max_flow : float. Defaults to 1.0
    The max magnitude of any flow vector in in_flow field.

Inputs
------
in_flow : ImageView
    rg32f image. The input optical flow used for the prediction.

Outputs
-------
out_flow : ImageView
    rg32f image. Predicted optical flow.

]]

function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = builder.name

    desc:addPort(ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('max_flow', 1.0)

    return desc
end


function builder.onNodeInit(node)

    local max_flow = node.descriptor:getParameter('max_flow')
    ll.logd(node.descriptor.builderName, 'onNodeInit: max_flow:', max_flow)

    -- max_flow could be any floating number. The round-up value
    -- is the number of iterations to propagate.
    local N = math.ceil(max_flow)

    local dt = 1.0 / N
    ll.logd(node.descriptor.builderName, 'onNodeInit: dt:', dt)

    local pushConstants = ll.PushConstants.new()
    pushConstants.float = dt

    local in_flow = node:getPort('in_flow')

    -- Pass through the input to the output
    if N == 0 then
        node:bind('out_flow', in_flow)
    end

    for i = 1, N do
        ll.logd(node.descriptor.builderName, 'iteration: ', i)

        local predictX = ll.createComputeNode('lluvia/opticalflow/flowfilter/FlowPredictX')
        local predictY = ll.createComputeNode('lluvia/opticalflow/flowfilter/FlowPredictY')

        predictX:setParameter('dt', dt)
        predictX:bind('in_flow', in_flow)
        predictX:init()

        predictY:setParameter('dt', dt)
        predictY:bind('in_flow', predictX:getPort('out_flow'))
        predictY:init()

        in_flow = predictY:getPort('out_flow')

        node:bindNode(string.format('FlowPredictX_%d', i), predictX)
        node:bindNode(string.format('FlowPredictY_%d', i), predictY)
    end

    -- in_flow in the last iteration contains the output of the last cycle.
    node:bind('out_flow', in_flow)

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')
end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd(node.descriptor.builderName, 'onNodeRecord')

    local max_flow = node.descriptor:getParameter('max_flow')
    local N = math.ceil(max_flow)

    for i = 1, N do
        ll.logd(node.descriptor.builderName, 'onNodeRecord: iteration:', i)

        local predictX = node:getNode(string.format('FlowPredictX_%d', i))
        local predictY = node:getNode(string.format('FlowPredictY_%d', i))

        predictX:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
        predictY:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
    end

    ll.logd(node.descriptor.builderName, 'onNodeRecord: finish')
end


ll.registerNodeBuilder(builder)
