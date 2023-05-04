local builder = ll.class(ll.ContainerNodeBuilder)

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

    -- Create a Lua array containing the RGBA values for the Viridis map. Could encode using base 64
    -- Create a ll.Buffer and transfer the RGBA content to it. Might need to support buffer.map here
    -- Copy the a RGBA ll.Image with the buffer's content
    -- Create a normalized ll.ImageView using linear interpolation
    -- Use the image view in the compute shader

    -- Need to check if the color map exists
    local encodedColorMap = builder.colorMaps[color_map]

    -- should map to std::vector<uint8_t>
    local decodedColorMap = ll.fromBase64(encodedColorMap)
    local bufferSize = decodedColorMap:size()

    local hostMemory = ll.getHostMemory()
    local stagingBuffer = hostMemory:createBuffer(bufferSize)
    stagingBuffer:mapAndSetFromVectorUint8(encodedColorMap)

    local textureMemory = ll.getDeviceMemory()
    local colorMapImage = textureMemory:createImage(
        ll.ImageDescriptor.new(1, 256, 1, ll.ChannelCount.C4, ll.ChannelType.Uint8),
        ll.ImageLayout.General,
        ll.MemoryPropertyFlags.Storage | ll.MemoryPropertyFlags.Sampled | ll.MemoryPropertyFlags.TransferDst)

    local colorMapImageView = colorMapImage:createImageView(
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Linear, true, true))

    local cmdBuffer = ll:CommandBuffer:new()

    -- begin
    cmdBuffer:begin()
    cmdBuffer:changeImageLayout(colorMapImage, ll.ImageLayout.TransferDstOptimal)
    cmdBuffer:copyBufferToImage(stagingBuffer, colorMapImage)
    cmdBuffer:changeImageLayout(colorMapImage, ll.ImageLayout.ShaderReadOnlyOptimal)
    cmdBuffer:end()
    cmdBuffer:run()
    -- end

    local colorMapNode = ll.createComputeNode('lluvia/viz/colormap/ColorMap_float')
    node:bindNode('ColorMap', colorMapNode)

    colorMapNode:setParameter('min_value', min_value)
    colorMapNode:setParameter('max_value', max_value)
    colorMapNode:setParameter('alpha', alpha)

    colorMapNode:bind('in_image', in_image)
    colorMapNode:bind('in_colormap', colorMapImageView)
    colorMapNode:init()

    node:bind('out_rgba', colorMapNode:getPort('out_rgba'))
end

function builder.onNodeRecord(node, cmdBuffer)

    ll.logd(node.descriptor.builderName, 'onNodeRecord')

    local colorMapNode = node:getNode('ColorMap')
    cmdBuffer:run(colorMapNode)
    
    ll.logd(node.descriptor.builderName, 'onNodeRecord: finish')
end

ll.registerNodeBuilder(builder)
