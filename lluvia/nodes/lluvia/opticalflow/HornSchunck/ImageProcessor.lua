local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/opticalflow/HornSchunck/ImageProcessor'
builder.doc = [[
Computes the parameters from the input image.

The layout of the `out_image_params` vector is as follows:

* `out_image_params.xy`: The X and Y gradient components of in_gray.
    The values are normalized to the range [-1, 1]
* `out_image_params.z`: The time difference (in_gray - in_gray_old)
* `out_image_params.w`: TODO

Parameters
----------
alpha_square : float. Defaults to TODO.
    TODO

float_precision : int. Defaults to ll.FloatPrecision.FP32.
    Floating point precision used accross the algorithm. The outputs out_gray and
    out_gradient will be of this floating point precision.

Inputs
------
in_gray : ImageView.
    r8ui image.

in_gray_old: ImageView.
    {r16f, r32f} image. The low-pass filtered version of in_gray.
    The values are normalized to the range [0, 1]

Outputs
-------
out_image_params : ImageView
    {rgba16f, rgba32f} image. The low-pass filtered version of in_gray.
    The values are normalized to the range [0, 1]

out_gray: ImageView
    {rg16f, rg32f} image. TODO

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
    desc:addPort(ll.PortDescriptor.new(2, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(3, 'out_image_params', ll.PortDirection.Out, ll.PortType.ImageView))

    desc:setParameter('alpha_square', 0.01)
    desc:setParameter('float_precision', ll.FloatPrecision.FP32)

    return desc
end

function builder.onNodeInit(node)
    ll.logd(node.descriptor.builderName, 'onNodeInit')

    local in_gray = node:getPort('in_gray')

    local alpha_square = node:getParameter('alpha_square')
    local pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(alpha_square)

    local float_precision = node:getParameter('float_precision')
    local outChannelType = ll.floatPrecisionToImageChannelType(float_precision)

    -- ll::Memory where out_image_params will be allocated
    local height = in_gray.height
    local width  = in_gray.width
    local memory = in_gray.memory

    local outGrayImgDesc = ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C1, outChannelType)
    local outImageParamsImgDesc = ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C4, outChannelType)

    -- normalizedCoordinates : false
    -- isSampled             : false
    local imgViewDesc = ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false)

    -- memory allocation
    local out_gray = memory:createImageView(outGrayImgDesc, imgViewDesc)
    local out_image_params = memory:createImageView(outImageParamsImgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_gray:changeImageLayout(ll.ImageLayout.General)
    out_image_params:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gray', out_gray)
    node:bind('out_image_params', out_image_params)

    node.pushConstants = pushConstants

    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')
end

ll.registerNodeBuilder(builder)
