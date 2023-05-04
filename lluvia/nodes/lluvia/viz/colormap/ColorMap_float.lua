local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/viz/colormap/ColorMap_float'
builder.doc = [[
TODO

Parameters
----------
min_value : float. Defaults to 0.0.
    The minimum value of the input image.

max_value : float. Defaults to 1.0.
    The maximum value of the input image.

alpha : float. Defaults to 1.0.
    The alpha value of the output image.

Inputs
------
in_image : ImageView.
    {r16f, r32f} image. Input image.

in_colormap : ImageView.
    {rgba8ui} image. The color map to use.

Outputs
-------
out_image : ImageView
    rgba8ui image. The encoded color of the optical flow field.

]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_image = ll.PortDescriptor.new(0, 'in_image', ll.PortDirection.In, ll.PortType.ImageView)
    in_image:checkImageChannelCountIs(ll.ChannelCount.C1)
    in_image:checkImageChannelTypeIsAnyOf({ll.ChannelType.Float16, ll.ChannelType.Float32})

    local in_colormap = ll.PortDescriptor.new(1, 'in_colormap', ll.PortDirection.In, ll.PortType.ImageView)
    in_colormap:checkImageChannelCountIs(ll.ChannelCount.C4)
    in_colormap:checkImageChannelTypeIs(ll.ChannelType.Uint8)

    desc:addPort(in_image)
    desc:addPort(in_colormap)
    desc:addPort(ll.PortDescriptor.new(2, 'out_rgba', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('min_value', 0.0)
    desc:setParameter('max_value', 1.0)
    desc:setParameter('alpha', 1.0)

    return desc
end

function builder.onNodeInit(node)
    
    local in_image = node:getPort('in_image')

    local min_value = node:getParameter('min_value')
    local max_value = node:getParameter('max_value')
    local alpha = node:getParameter('alpha')
    ll.logd(node.descriptor.builderName, string.format('min_value: %f, max_value: %f, alpha: %f', min_value, max_value, alpha))

    local pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(min_value)
    pushConstants:pushFloat(max_value)
    pushConstants:pushFloat(alpha)
    node.pushConstants = pushConstants

    local memory = in_image.memory

    out_rgba = memory:createImageView(
        ll.ImageDescriptor.new(1, in_image.height, in_image.width, ll.ChannelCount.C4, ll.ChannelType.Uint8),
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false))

    out_rgba:changeImageLayout(ll.ImageLayout.General)
    out_rgba:clear()

    node:bind('out_rgba', out_rgba)
    node:configureGridShape(ll.vec3ui.new(in_image.width, in_image.height, 1))
end

ll.registerNodeBuilder(builder)
