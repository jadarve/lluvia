local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/viz/Flow2RGBA'
builder.doc = [[
Encodes a 2D optical flow field as RGBA color.

The encoding uses the HSV color wheel to encode angle, with some modifications:

* Saturation increases linearly with the norm
* Value increases logaritmically with the norm.
* For norm equal zero, the RGB value is black
* For vector norms greater than max_flow, set value to 0.8.
* Invalid pixels are coloured white.

Parameters
----------
max_flow : float. Defaults to 1.0.
    The maximum norm of any vector in the field.

Inputs
------
in_flow : ImageView.
    {rg16f, rg32f} image. Input optical flow

Outputs
-------
out_rgba : ImageView
    rgba8ui image. The encoded color of the optical flow field.

]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_flow = ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView)
    in_flow:checkImageChannelCountIs(ll.ChannelCount.C2)
    in_flow:checkImageChannelTypeIsAnyOf({ll.ChannelType.Float16, ll.ChannelType.Float32})

    desc:addPort(in_flow)
    desc:addPort(ll.PortDescriptor.new(1, 'out_rgba', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('max_flow', 1.0)

    return desc
end

function builder.onNodeInit(node)
    
    local in_flow = node:getPort('in_flow')

    local pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(node:getParameter('max_flow'))
    node.pushConstants = pushConstants

    local memory = in_flow.memory

    out_rgba = memory:createImageView(
        ll.ImageDescriptor.new(1, in_flow.height, in_flow.width, ll.ChannelCount.C4, ll.ChannelType.Uint8),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))

    out_rgba:changeImageLayout(ll.ImageLayout.General)
    out_rgba:clear()

    node:bind('out_rgba', out_rgba)
    node:configureGridShape(ll.vec3ui.new(in_flow.width, in_flow.height, 1))
end

ll.registerNodeBuilder(builder)
