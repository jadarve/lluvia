local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/FlowUpdateDelta'
builder.doc = [[
Computes one step update of the optical flow based on image data and previous estimate.

Parameters
----------
gamma : float. Defaults to 0.01
    The filter gains for the update step.

maxflow : float. Defaults to 1.0
    The max magnitude allowed for the optical flow output.

Inputs
------
in_gray : ImageView
    r32f image. The input gray-scale image at current time step.

in_gradient : ImageView
    rg32f image. The input gray-scale image gradient at current time step.

in_delta_flow : Imageview
    rg32f image. The current estimation of the optical flow.

in_gray_old : ImageView
    r32f image. The input gray-scale image at current time step.

in_flow : Imageview
    rg32f image. The current estimation of the optical flow from a level
    above in the pyramid. This image view has half the resolution
    of the other inputs.

Outputs
-------
out_gray : ImageView
    r32f image. The image estimate for the next time step. It is a memory copy
    of in_gray. This image is allocated externally in FlowFilterDelta and bound
    to this node. This way, the loop between FlowPredictPayload and FlowUpdateDelta
    can be broken.

out_flow : ImageView
    rg32f image. The updated optical flow.

out_delta_flow : ImageView
    rg32f image. The updated optical flow. This image is allocated externally in
    FlowFilterDelta and bound to this node. This way, the loop between
    FlowPredictPayload and FlowUpdateDelta can be broken.
]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'in_gradient', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'in_delta_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(3, 'in_gray_old', ll.PortDirection.In, ll.PortType.ImageView))

    desc:addPort(ll.PortDescriptor.new(4, 'in_flow', ll.PortDirection.In, ll.PortType.SampledImageView))

    desc:addPort(ll.PortDescriptor.new(5, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(6, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(7, 'out_delta_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('gamma', 0.01)
    desc:setParameter('maxflow', 1.0)

    return desc
end

function builder.onNodeInit(node)
    ll.logd(node.descriptor.builderName, 'onNodeInit')

    local gamma = node:getParameter('gamma')
    local maxflow = node:getParameter('maxflow')

    local pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(gamma)
    pushConstants:pushFloat(maxflow)

    local in_flow = node:getPort('in_flow')
    local in_gray = node:getPort('in_gray')

    -- ll::Memory where out_flow will be allocated
    local memory = in_flow.memory

    local outFlowImgDesc = ll.ImageDescriptor.new(in_flow.imageDescriptor)
    outFlowImgDesc.width = in_gray.width
    outFlowImgDesc.height = in_gray.height

    local outFlowViewDesc = ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false)
    
    local out_flow = memory:createImageView(outFlowImgDesc, outFlowViewDesc)
    out_flow:changeImageLayout(ll.ImageLayout.General)
    node:bind('out_flow', out_flow)

    local out_delta_flow = node:getPort('out_delta_flow')
    ll.logd(node.descriptor.builderName, 'in_flow ', string.format('[%d, %d, %d]', in_flow.width, in_flow.height, in_flow.channelCount)
                             , 'out_flow', string.format('[%d, %d, %d]', out_flow.width, out_flow.height, out_flow.channelCount)
                             , 'out_delta_flow', string.format('[%d, %d, %d]', out_delta_flow.width, out_delta_flow.height, out_delta_flow.channelCount))

    node.pushConstants = pushConstants
    node:configureGridShape(ll.vec3ui.new(out_flow.width, out_flow.height, 1))

    -- clear outputs
    node:getPort('out_gray'):clear()
    node:getPort('out_flow'):clear()
    node:getPort('out_delta_flow'):clear()

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')
end

ll.registerNodeBuilder(builder)
