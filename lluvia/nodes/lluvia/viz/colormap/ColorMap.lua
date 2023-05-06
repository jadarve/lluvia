local builder = ll.class(ll.ContainerNodeBuilder)

builder.name = 'lluvia/viz/colormap/ColorMap'
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

alpha : float. Defaults to 0.0.
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
builder.colorMaps = {}
builder.colorMaps['Viridis'] =  'RQJVAEUDVgBFBFgARgZZAEYHWwBGCVwARwpdAEcMXwBHDWAARw9iAEgQYwBIEmQASBNmAEgVZwBIFmgASBdqAEgZawBJGmwASRttAEkdbwBJHnAASR9xAEkhcgBJInMASSN0AEkkdQBIJnYASCd3AEgoeABIKXkASCt6AEgsewBILXwARy59AEcwfQBHMX4ARzJ/AEYzgABGNYAARjaBAEY3ggBFOIIARTqDAEQ7hABEPIQARD2FAEM+hQBDP4YAQ0GGAEJChwBCQ4cAQUSIAEFFiABARogAQEiJAD9JiQA/SooAPkuKAD5MigA+TYoAPU6LAD1PiwA8UYsAPFKLADtTjAA7VIwAOlWMADpWjAA5V4wAOViNADhZjQA4Wo0AN1uNADdcjQA2XY0ANl6NADVfjgA1YI4ANGGOADRijgAzY44AM2SOADJljgAyZo4AMmeOADFojgAxaY4AMGqOADBrjgAvbI8AL22PAC9ujwAub48ALnCPAC1xjwAtco8ALXOPACx0jwAsdY8AK3aPACt3jwAreI8AKnmPACp6jwApe48AKXuPACl8jwAofY8AKH6PACh/jwAngI8AJ4GPACeCjwAmg48AJoSOACWFjgAlho4AJYeOACSIjgAkiY4AJIqOACOKjgAji44AI4yOACKNjgAijo0AIo+NACGQjQAhkY0AIZKNACCTjQAglIwAIJWMACCWjAAgl4wAH5iLAB+ZiwAfmosAH5qLAB+bigAfnIoAH52KAB+eiQAfn4kAH6CJAB+hiAAgoogAIKOHACCkhwAhpYYAIaaGACKnhgAiqIUAI6iFACSphAAkqoMAJauDACasggAnrYIAKK6BACmvgAAqsIAAK7F/ACyyfgAtsn4AL7N9ADC0fAAxtXsAM7Z7ADS3egA2uHkAN7l4ADm6dwA6uncAPLt2AD68dQA/vXQAQb5zAEO/cgBFv3EARsBwAEjBbwBKwm4ATMNtAE7DbABQxGoAUsVpAFTGaABWx2cAWMdmAFrIZQBcyWMAX8piAGHKYQBjy2AAZcxeAGjNXQBqzVwAbM5aAG7PWQBxz1cAc9BWAHXRVQB40VMAetJSAH3TUAB/008AgtRNAITUTACH1UoAidZIAIzWRwCO10UAkddEAJPYQgCW2EAAmNk/AJvZPQCe2jsAoNo5AKPbOACm2zYAqNw0AKvcMwCu3TEAsN0vALPeLQC23iwAuN4qALvfKAC+3ycAwOAlAMPgIwDG4CIAyOEgAMvhHwDO4R4A0OIdANPiHADV4hsA2OMaANvjGQDd4xkA4OQZAOLkGQDl5BkA6OUaAOrlGgDt5RsA7+YcAPLmHQD05h8A9+cgAPnnIgD75yMA/uglAA=='

builder.colorMapNodeMap = {}
builder.colorMapNodeMap[ll.ChannelType.Uint8] = 'lluvia/viz/colormap/ColorMap_uint'
builder.colorMapNodeMap[ll.ChannelType.Uint16] = 'lluvia/viz/colormap/ColorMap_uint'
builder.colorMapNodeMap[ll.ChannelType.Uint32] = 'lluvia/viz/colormap/ColorMap_uint'
builder.colorMapNodeMap[ll.ChannelType.Int8] = 'lluvia/viz/colormap/ColorMap_int'
builder.colorMapNodeMap[ll.ChannelType.Int16] = 'lluvia/viz/colormap/ColorMap_int'
builder.colorMapNodeMap[ll.ChannelType.Int32] = 'lluvia/viz/colormap/ColorMap_int'
builder.colorMapNodeMap[ll.ChannelType.Float16] = 'lluvia/viz/colormap/ColorMap_float'
builder.colorMapNodeMap[ll.ChannelType.Float32] = 'lluvia/viz/colormap/ColorMap_float'

function builder.newDescriptor() 
    
    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = builder.name

    local in_image = ll.PortDescriptor.new(0, 'in_image', ll.PortDirection.In, ll.PortType.ImageView)
    in_image:checkImageChannelCountIs(ll.ChannelCount.C1)

    -- add support for all channel types
    in_image:checkImageChannelTypeIsAnyOf({ll.ChannelType.Uint8, ll.ChannelType.Int8, ll.ChannelType.Float16, ll.ChannelType.Float32})

    desc:addPort(in_image)
    desc:addPort(ll.PortDescriptor.new(1, 'out_rgba', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('colormap', 'Viridis')
    desc:setParameter('min_value', 0.0)
    desc:setParameter('max_value', 1.0)
    desc:setParameter('alpha', 0.0)

    return desc
end

function builder.onNodeInit(node)
    
    local in_image = node:getPort('in_image')

    local colormap = node:getParameter('colormap')
    local min_value = node:getParameter('min_value')
    local max_value = node:getParameter('max_value')
    local alpha = node:getParameter('alpha')
    ll.logd(node.descriptor.builderName, string.format('colormap: %s, min_value: %f, max_value: %f, alpha: %f', colormap, min_value, max_value, alpha))

    -- FIXME
    colormap = 'Viridis'

    -- Create a Lua array containing the RGBA values for the Viridis map. Could encode using base 64
    -- Create a ll.Buffer and transfer the RGBA content to it. Might need to support buffer.map here
    -- Copy the a RGBA ll.Image with the buffer's content
    -- Create a normalized ll.ImageView using linear interpolation
    -- Use the image view in the compute shader

    -- Need to check if the color map exists
    local encodedColorMap = builder.colorMaps[colormap]
    if encodedColorMap == nil then
        error(node.descriptor.builderName .. ': color map not found: ' .. color_map)
    end

    -- should map to std::vector<uint8_t>
    local decodedColorMap = ll.fromBase64(encodedColorMap)
    local bufferSize = decodedColorMap:size()
    ll.logd(node.descriptor.builderName, string.format('bufferSize: %d', bufferSize))

    local hostMemory = ll.getHostMemory()
    local stagingBuffer = hostMemory:createBuffer(bufferSize)
    stagingBuffer:mapAndSetFromVectorUint8(decodedColorMap)

    local textureMemory = ll.getDeviceMemory()
    local colorMapImage = textureMemory:createImage(
        ll.ImageDescriptor.new(1, 1, 256, ll.ChannelCount.C4, ll.ChannelType.Uint8),
        ll.ImageLayout.General,
        ll.ImageUsageFlagBits.Storage | ll.ImageUsageFlagBits.Sampled | ll.ImageUsageFlagBits.TransferDst)

    local colorMapImageView = colorMapImage:createImageView(
        ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Linear, true, true))

    local cmdBuffer = ll.createCommandBuffer()
    cmdBuffer:begin()
    cmdBuffer:changeImageLayout(colorMapImage, ll.ImageLayout.TransferDstOptimal)
    cmdBuffer:copyBufferToImage(stagingBuffer, colorMapImage)
    cmdBuffer:changeImageLayout(colorMapImage, ll.ImageLayout.ShaderReadOnlyOptimal)
    cmdBuffer:ends()

    ll.runCommandBuffer(cmdBuffer)

    local colorMapNodePath = builder.colorMapNodeMap[in_image.channelType]
    if colorMapNodePath == nil then
        error(node.descriptor.builderName .. ': unsupported channel type: ' .. in_image.channelType)
    end

    local colorMapNode = ll.createComputeNode(colorMapNodePath)
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
