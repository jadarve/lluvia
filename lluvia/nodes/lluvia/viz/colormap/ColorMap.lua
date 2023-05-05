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
builder.colorMaps = {}
builder.colorMaps['Gray'] =  'IUBFKRIDKZCQIWCGAZMUMB23IYEVYRYKLVDQYX2HBVQEOD3CJAIGGSASMREBGZSICVTUQFTIJALWUSAZNNERU3CJDNWUSHLPJEPHASI7OFESC4SJEJZUSI3UJESHKSBGOZECO52IFB4EQKLZJAVXUSBMPNEC27CHFZ6UOMD5I4YX4RZSP5DDHACGGWAEMNUBIY3YERJYQJCTVA2EHOCEIPEEIQ6YKQZ6QVBT7BSDIGDEEQUHIJBYOQKERBAULCCAI2EEASEJH5EYSP2KRI7EXCR6JSFD4TMKHVHIWPKPRM6FDCZ4KKFTWU4MHNKIYOSVRQ5FNDBZK6GDSWENHBMY2OC2RU3VXDJXLSGTMXMNGZPI2NK7RY2WBDRUMGHDIYUOGNRY4M3ERYZGLDRSM2HDEZ4OGFUI4MLJRYYGVDRQNOHC63EPF5WY6L3OR4XG7DZOOCHS24MPFVZI6LLTR4WHJDZMOWHSW5UPFN3Y6K3YR4VHTDZKPKHSS64PFF5Y6KL4R4UH3DZIP2HSQ74PE6AI6J4BR4TYFDZGQOHSNBEOEWCY4JMGRYSYPDRERCHCJCMOESFI4I4KRYRYXDRDRSHCFDMOEKHI2IUPRUQZBDJBSGGSDEUNECJY2IEURQQJLDBAS2GCBF4MD6MIWH4ZRMPZVCY7TKFR7G4KD6OIUH45RIPZ5CI7T6ER7IEJD6QYQIFCRAQKHBZAUSDSDJMGEGTIMIVHQYRKRBJDVCCSJKMEESVIGJNLQMTKZARHVWBCRLUBFGXYAKVQQAV3C7ZMWJ7C3MT6F6ZX2MFUPQY3K6ZTWZ5TJN32G24HSN5ZPA43U5Z2XJ3TZO3WH26HKP55ORA3442DX5ZELP3RI3AHASGBN5FME3SMYNWU5Q3MKDCGUUWFNFKMM2CWY5TVRR3GLLEGKXGJMNP4UYTBZJQWHS3AMXGF42GNLVVM2XDMZZNG5T2ZOHHVO46QKZ25CVLY2FJXVUSSPXJVA76TJ6BNITME2RGIPVKKRHLERDGWI6HNORMR25CJHWCCS3MEBGGZH6N5SPM63I52BWRZUPNTRJW3G2UNYNFL3QZ25XJRWDOS7M66FW3N4LFY3YVLXXZIX3PSPQHAEXB6AI6G4ARMRYJAZPQR7TXBD3IOEHOT4IONLYQ33DRRVW7DDHO6GGPA4QM6FZAZ4XSBT2HFDLVOKGXN4UN67ZQ46LTB35HGD736OIHZ44RPXZZD73UCK==='


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

    local cmdBuffer = ll.CommandBuffer.new()
    cmdBuffer:begin()
    cmdBuffer:changeImageLayout(colorMapImage, ll.ImageLayout.TransferDstOptimal)
    cmdBuffer:copyBufferToImage(stagingBuffer, colorMapImage)
    cmdBuffer:changeImageLayout(colorMapImage, ll.ImageLayout.ShaderReadOnlyOptimal)
    cmdBuffer:ends()
    cmdBuffer:run()

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
