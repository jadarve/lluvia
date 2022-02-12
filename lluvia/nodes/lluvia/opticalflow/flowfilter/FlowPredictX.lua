local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/FlowPredictX'
builder.doc = [[
Computes one step predition of the optical flow along the X axis.

Parameters
----------
dt : float. Defaults to 1.0
    The time step used for the prediction.

Inputs
------
in_flow : ImageView
    {rg16f, rg32f} image. The input optical flow used for the prediction.

Outputs
-------
out_flow : ImageView
    {rg16f, rg32f} image. Predicted optical flow. The floating point precision
    will be the same as in_flow.

]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_flow = ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView)
    in_flow:checkImageChannelCountIs(ll.ChannelCount.C2)
    in_flow:checkImageChannelTypeIsAnyOf({ll.ChannelType.Float16, ll.ChannelType.Float32})

    desc:addPort(in_flow)
    desc:addPort(ll.PortDescriptor.new(1, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('dt', 1.0)

    return desc
end

function builder.onNodeInit(node)

    local dt = node:getParameter('dt')
    ll.logd('FlowPredictX', 'onNodeInit: dt', dt)

    local pushConstants = ll.PushConstants.new()
    pushConstants.float = dt
    node.pushConstants = pushConstants

    local in_flow = node:getPort('in_flow')

    -- ll::Memory where out_flow will be allocated
    local memory = in_flow.memory

    local out_flow = memory:createImageView(in_flow.imageDescriptor, in_flow.descriptor)

    -- need to change image layout before binding
    out_flow:changeImageLayout(ll.ImageLayout.General)
    out_flow:clear()

    node:bind('out_flow', out_flow)

    ll.logd('FlowPredictX', 'in_flow ', string.format('[%d, %d, %d]', in_flow.width, in_flow.height, in_flow.channelCount)
                              , 'out_flow', string.format('[%d, %d, %d]', out_flow.width, out_flow.height, out_flow.channelCount))

    node:configureGridShape(ll.vec3ui.new(out_flow.width, out_flow.height, 1))

    ll.logd('FlowPredictX', 'onNodeInit: finish')
end

ll.registerNodeBuilder(builder)
