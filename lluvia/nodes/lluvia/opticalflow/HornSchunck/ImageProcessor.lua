local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/opticalflow/HornSchunck/ImageProcessor'
builder.doc = [[
Computes the parameters from the input image sequence.

The layout of the `out_image_params` vector is as follows:

* `out_image_params.xy`: The X and Y gradient components of in_gray.
    The values are normalized to the range [-1, 1]
* `out_image_params.z`: The time difference (in_gray - in_gray_old)
* `out_image_params.w`: The regularization value `1.0 / (alpha_square + grad_x*grad_x + grad_y*grad_y)`

Parameters
----------
alpha : float. Defaults to 0.05.
    Regularization gain.

float_precision : int. Defaults to ll.FloatPrecision.FP32.
    Floating point precision used accross the algorithm. The out_image_params
    output will be of this floating point precision.

Inputs
------
in_gray : ImageView.
    r8ui image.

in_gray_old: ImageView.
    {r16f, r32f} image. The low-pass filtered version of in_gray.
    The values are normalized to the range [0, 1].

Outputs
-------
out_image_params : ImageView
    {rgba16f, rgba32f} image. Output image parameters
]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_gray = ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView)
    in_gray:checkImageChannelCountIs(ll.ChannelCount.C1)
    in_gray:checkImageChannelTypeIs(ll.ChannelType.Uint8)

    local in_gray_old = ll.PortDescriptor.new(1, 'in_gray_old', ll.PortDirection.In, ll.PortType.ImageView)
    in_gray_old:checkImageChannelCountIs(ll.ChannelCount.C1)
    in_gray_old:checkImageChannelTypeIsAnyOf({ll.ChannelType.Float16, ll.ChannelType.Float32})

    desc:addPort(in_gray)
    desc:addPort(in_gray_old)
    desc:addPort(ll.PortDescriptor.new(2, 'out_image_params', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('alpha', 0.05)
    desc:setParameter('float_precision', ll.FloatPrecision.FP32)

    return desc
end

function builder.onNodeInit(node)
    ll.logd(node.descriptor.builderName, 'onNodeInit')

    local in_gray = node:getPort('in_gray')
    local in_gray_old = node:getPort('in_gray_old')

    local alpha = node:getParameter('alpha')
    local pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(alpha*alpha) -- push alpha square

    local float_precision = node:getParameter('float_precision')
    local outChannelType = ll.floatPrecisionToImageChannelType(float_precision)

    -- ll::Memory where out_image_params will be allocated
    local memory = in_gray.memory
    
    local height = in_gray.height
    local width  = in_gray.width

    local outImageParamsImgDesc = ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C4, outChannelType)

    -- normalizedCoordinates : false
    -- isSampled             : false
    local imgViewDesc = ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false)

    -- memory allocation
    local out_image_params = memory:createImageView(outImageParamsImgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_image_params:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_image_params', out_image_params)

    node.pushConstants = pushConstants

    node:configureGridShape(ll.vec3ui.new(in_gray.width, in_gray.height, 1))

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')
end

ll.registerNodeBuilder(builder)
