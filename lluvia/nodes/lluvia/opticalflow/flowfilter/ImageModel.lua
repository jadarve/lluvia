local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/ImageModel'
builder.doc = [[
Computes the image model from an in_gray image.

The outputs are a low-pass filtered version of in_gray using a 3x3 Gaussian filter,
and a 2D image representing the XY partial differences of the low-pass image.

Inputs
------
in_gray : ImageView.
    r8ui image.

Outputs
-------
out_gray : ImageView
    r32f image. The low-pass filtered version of in_gray.
    The values are normalized to the range [0, 1]

out_gradient: ImageView
    rg32f image. The X and Y gradient components of in_gray.
    The values are normalized to the range [-1, 1]

]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

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

ll.registerNodeBuilder(builder)
