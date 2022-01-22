local builder = ll.class(ll.ContainerNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/FlowPredictPayload'
builder.doc = [[
Computes a forward prediction of an optical flow field and payloads.

The prediction is computed as a series of X and Y predictions using
the FlowPredictX and FlowPredictY compute nodes.

Based on the value of maxflow parameter, the time step dt is calculated as:

dt = 1.0 / ceil(maxflow)

Parameters
----------
maxflow : float. Defaults to 1.0
    The max magnitude of any flow vector in in_flow field.

Inputs
------
in_flow : ImageView
    rg32f image. The input optical flow used for the prediction.

in_gray : ImageView
    r32f image. Input scalar field payload.

in_vector : Imageview
    rg32f image. Input vector field payload.

Outputs
-------
out_flow : ImageView
    rg32f image. Predicted optical flow.

out_gray : ImageView
    r32f image. Predicted scalar field.

out_vector : ImageView
    rg32f image. Predicted vector field.

]]

function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = builder.name

    desc:addPort(ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'in_vector', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(3, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(4, 'out_vector', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(5, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('maxflow', 4)

    return desc
end


function builder.onNodeInit(node)

    local maxflow = node.descriptor:getParameter('maxflow')
    ll.logd(node.descriptor.builderName, 'onNodeInit: maxflow:', maxflow)

    -- maxflow could be any floating number. The round-up value
    -- is the number of iterations to predict.
    local N = math.ceil(maxflow)

    local dt = 1.0 / N
    ll.logd(node.descriptor.builderName, 'onNodeInit: dt:', dt)

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
        ll.logd(node.descriptor.builderName, 'iteration: ', i)

        local predictX = ll.createComputeNode('lluvia/opticalflow/flowfilter/FlowPredictPayloadX')
        local predictY = ll.createComputeNode('lluvia/opticalflow/flowfilter/FlowPredictPayloadY')

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

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')
end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd(node.descriptor.builderName, 'onNodeRecord')

    local maxflow = node.descriptor:getParameter('maxflow')
    local N = math.ceil(maxflow)

    for i = 1, N do
        ll.logd(node.descriptor.builderName, 'onNodeRecord: iteration:', i)

        local predictX = node:getNode(string.format('FlowPredictPayloadX_%d', i))
        local predictY = node:getNode(string.format('FlowPredictPayloadY_%d', i))

        predictX:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
        predictY:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
    end

    ll.logd(node.descriptor.builderName, 'onNodeRecord: finish')
end


ll.registerNodeBuilder(builder)
