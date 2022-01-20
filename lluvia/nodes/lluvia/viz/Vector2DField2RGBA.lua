local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/viz/Vector2DField2RGBA'
builder.doc = [[
Encodes a 2D field of 2D vectors to RGBA color space.

The encoding uses the HSV color wheel to encode angle, with some modifications:

* Saturation increases linearly with the norm
* Value increases logaritmically with the norm.
* For norm equal zero, the RGB value is black
* For vector norms greater than max_norm, set value to 0.8.
* Invalid pixels are coloured white.

Parameters
----------
max_norm : float. Defaults to 1.0.
    The maximum norm of any vector in the field.

Inputs
------
in_vec2DField : ImageView.
    rg32f image.

Outputs
-------
out_rgba : ImageView
    rgba8ui image. The encoded color of the vector field.

]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    desc:addPort(ll.PortDescriptor.new(0, 'in_vec2DField', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_rgba', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('max_norm', 1.0)

    return desc
end

function builder.onNodeInit(node)
    
    local in_vec2DField = node:getPort('in_vec2DField')

    local pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(node:getParameter('max_norm'))
    node.pushConstants = pushConstants

    local memory = in_vec2DField.memory

    out_rgba = memory:createImageView(
        ll.ImageDescriptor.new(1, in_vec2DField.height, in_vec2DField.width, ll.ChannelCount.C4, ll.ChannelType.Uint8),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))

    out_rgba:changeImageLayout(ll.ImageLayout.General)
    out_rgba:clear()

    node:bind('out_rgba', out_rgba)
    node:configureGridShape(ll.vec3ui.new(in_vec2DField.width, in_vec2DField.height, 1))
end

ll.registerNodeBuilder(builder)
