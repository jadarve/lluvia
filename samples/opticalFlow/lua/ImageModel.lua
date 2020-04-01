local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'ImageModel'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('ImageModel')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'out_gradient', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)
    ll.logd('ImageModel', 'onNodeInit', 'start')

    local in_gray = node:getPort('in_gray')

    -- ll::Memory where out_prefilter will be allocated
    local memory = in_gray.memory

    -- out_gray descriptors
    ll.logd('ImageModel', 'onNodeInit', 'creating image descriptors')
    local grayImgDesc = ll.ImageDescriptor.new(in_gray.imageDescriptor)
    grayImgDesc.channelCount = 1
    grayImgDesc.channelType = ll.ChannelType.Float32

    -- out_gradient descriptors
    local gradientImgDesc = ll.ImageDescriptor.new(in_gray.imageDescriptor)
    gradientImgDesc.channelCount = 2
    gradientImgDesc.channelType = ll.ChannelType.Float32

    -- the image view descriptor has the same parameters for both outputs
    ll.logd('ImageModel', 'onNodeInit', 'creating image view descriptor')
    local imgViewDesc = ll.ImageViewDescriptor.new()
    imgViewDesc:setAddressMode(ll.ImageAddressMode.MirroredRepeat)
    imgViewDesc.filterMode = ll.ImageFilterMode.Nearest
    imgViewDesc.isSampled = false
    imgViewDesc.normalizedCoordinates = false

    -- memory allocation
    ll.logd('ImageModel', 'onNodeInit', 'allocating images')
    local out_gray = memory:createImageView(grayImgDesc, imgViewDesc)
    local out_gradient = memory:createImageView(gradientImgDesc, imgViewDesc)

    -- need to change image layout before binding
    ll.logd('ImageModel', 'onNodeInit', 'changing layout')
    out_gray:changeImageLayout(ll.ImageLayout.General)
    out_gradient:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gray', out_gray)
    node:bind('out_gradient', out_gradient)
    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))

    ll.logd('ImageModel', 'onNodeInit', 'finish')
end

ll.registerNodeBuilder('ImageModel', builder)
