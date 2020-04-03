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
    local height = in_gray.height
    local width  = in_gray.width
    local memory = in_gray.memory

    local grayImgDesc = ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C1, ll.ChannelType.Float32)
    local gradientImgDesc = ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C2, ll.ChannelType.Float32)

    -- normalizedCoordinates : false
    -- isSampled             : false
    local imgViewDesc = ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false)

    -- memory allocation
    local out_gray = memory:createImageView(grayImgDesc, imgViewDesc)
    local out_gradient = memory:createImageView(gradientImgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_gray:changeImageLayout(ll.ImageLayout.General)
    out_gradient:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gray', out_gray)
    node:bind('out_gradient', out_gradient)
    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))

    ll.logd('ImageModel', 'onNodeInit', 'finish')
end

ll.registerNodeBuilder('ImageModel', builder)
