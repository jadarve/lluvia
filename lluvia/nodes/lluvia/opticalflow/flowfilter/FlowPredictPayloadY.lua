local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/FlowPredictPayloadY'
builder.doc = [[
Computes one step predition of the optical flow and payloads along the Y axis.

Parameters
----------
dt : float. Defaults to 1.0
    The time step used for the prediction.

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
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    desc:addPort(ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'in_vector', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(3, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(4, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(5, 'out_vector', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('dt', 1.0)

    return desc
end

function builder.onNodeInit(node)

    local dt = node:getParameter('dt')

    ll.logd('FlowPredictPayloadY', 'onNodeInit: dt', dt)

    local pushConstants = ll.PushConstants.new()
    pushConstants.float = dt
    node.pushConstants = pushConstants

    local in_flow = node:getPort('in_flow')
    local in_gray = node:getPort('in_gray')
    local in_vector = node:getPort('in_vector')

    -- ll::Memory where out_flow will be allocated
    local memory = in_flow.memory

    local out_flow = memory:createImageView(in_flow.imageDescriptor, in_flow.descriptor)
    local out_gray = memory:createImageView(in_gray.imageDescriptor, in_gray.descriptor)
    local out_vector = memory:createImageView(in_vector.imageDescriptor, in_vector.descriptor)

    -- need to change image layout before binding
    out_flow:changeImageLayout(ll.ImageLayout.General)
    out_gray:changeImageLayout(ll.ImageLayout.General)
    out_vector:changeImageLayout(ll.ImageLayout.General)

    out_flow:clear()
    out_gray:clear()
    out_vector:clear()

    node:bind('out_flow', out_flow)
    node:bind('out_gray', out_gray)
    node:bind('out_vector', out_vector)

    ll.logd('FlowPredictPayloadY', 'in_flow ', string.format('[%d, %d, %d]', in_flow.width, in_flow.height, in_flow.channelCount)
                              , 'out_flow', string.format('[%d, %d, %d]', out_flow.width, out_flow.height, out_flow.channelCount))

    node:configureGridShape(ll.vec3ui.new(out_flow.width, out_flow.height, 1))

    ll.logd('FlowPredictPayloadY', 'onNodeInit: finish')
end

ll.registerNodeBuilder(builder)
