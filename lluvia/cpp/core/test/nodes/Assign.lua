local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'Assign'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('Assign')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'outputBuffer', ll.PortDirection.Out, ll.PortType.Buffer))

    return desc
end

function builder.onNodeInit(node)

    local in_gray = node:getPort('in_gray')
    local memory = in_gray.memory

    local gradientImgDesc = ll.ImageDescriptor.new(1,
                                                   in_gray.height,
                                                   in_gray.width,
                                                   ll.ChannelCount.C2,
                                                   ll.ChannelType.Float32)

    -- normalizedCoordinates : false
    -- isSampled             : false
    local imgViewDesc = ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat,
                                                   ll.ImageFilterMode.Nearest,
                                                   false,
                                                   false)

    -- memory allocation
    local out_gradient = memory:createImageView(gradientImgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_gradient:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gradient', out_gradient)
    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))
end

ll.registerNodeBuilder('Sobel', builder)
