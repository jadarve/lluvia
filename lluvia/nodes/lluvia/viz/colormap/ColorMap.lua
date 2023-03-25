local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/viz/ColorMap_gray'
builder.doc = [[
Maps numeric values to colors.

Parameters
----------
color_map : string. Defaults to "Gray".
    The color map to use. Possible values are:
    - "Gray"
    - "Viridis"

min_value : float. Defaults to 0.0.
    The minimum value of the input image.

max_value : float. Defaults to 1.0.
    The maximum value of the input image.

alpha : float. Defaults to 1.0.
    The alpha value of the output image.

Inputs
------
in_image : ImageView.
    {r8ui, r8i, r16f, r32f} image. Input image.

Outputs
-------
out_image : ImageView
    rgba8ui image. The encoded color of the optical flow field.

]]

-- I could add attributes to the builder
builder.colorMaps = {
    'Gray': 'abcdf1234=='
}

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_image = ll.PortDescriptor.new(0, 'in_image', ll.PortDirection.In, ll.PortType.ImageView)
    in_image:checkImageChannelCountIs(ll.ChannelCount.C1)

    -- add support for all channel types
    in_image:checkImageChannelTypeIsAnyOf({ll.ChannelType.Uint8, ll.ChannelType.Int8, ll.ChannelType.Float16, ll.ChannelType.Float32})

    desc:addPort(in_image)
    desc:addPort(ll.PortDescriptor.new(1, 'out_rgba', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('color_map', 'Gray')
    desc:setParameter('min_value', 0.0)
    desc:setParameter('max_value', 1.0)
    desc:setParameter('alpha', 1.0)

    return desc
end

function builder.onNodeInit(node)
    
    local in_image = node:getPort('in_image')

    local color_map = node:getParameter('color_map')
    local min_value = node:getParameter('min_value')
    local max_value = node:getParameter('max_value')
    local alpha = node:getParameter('alpha')
    ll.logd(node.descriptor.builderName, string.format('color_map: %s, min_value: %f, max_value: %f, alpha: %f', color_map, min_value, max_value, alpha))

    if color_map == 'Gray' then
    else if color_map == 'Viridis' then

        -- Create a Lua array containing the RGBA values for the Viridis map. Could encode using base 64
        -- Create a ll.Buffer and transfer the RGBA content to it. Might need to support buffer.map here
        -- Copy the a RGBA ll.Image with the buffer's content
        -- Create a normalized ll.ImageView using linear interpolation
        -- Use the image view in the compute shader 
    else
        -- error
    end

    -- Need to check if the color map exists
    local encodedColorMap = builder.colorMaps[color_map]

    local hostMemory = ll.getHostMemory()
    local stagingBuffer = hostMemory:createBufferFromBase64(encodedColorMap)

    -- TODO: texture memory
    local textureMemory = ll.getDeviceMemory()
    local colorMap = textureMemory:createImage(
        ll.ImageDescriptor.new(1, 256, 1, ll.ChannelCount.C4, ll.ChannelType.Uint8),
        ll.ImageLayout.General,
        ll.MemoryPropertyFlags.HostVisible | ll.MemoryPropertyFlags.HostCoherent)

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
