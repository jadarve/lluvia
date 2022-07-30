local builder = ll.class(ll.ContainerNodeBuilder)

builder.name = 'lluvia/opticalflow/HornSchunck/HornSchunck'
builder.doc = [[
Horn and Schunck variational optical flow algorithm.

Parameters
----------
gamma : float. Defaults to 0.01
    The filter gains for the update step.

max_flow : float. Defaults to 1.0
    The max magnitude allowed for the optical flow output.

smooth_iterations : int. Defaults to 1.
    The number of smooth iterations to apply to the estimated flow.

float_precision : int. Defaults to ll.FloatPrecision.FP32.
    Floating point precision used accross the algorithm. The outputs out_gray
    and out_flow will be of this floating point precision.

Inputs
------
in_gray : ImageView
    r8ui image. The input gray-scale image.

Outputs
-------
out_gray : ImageView
    {r16f, r32f} image. The gray-scale image after one iteration of the algorithm.
    The image format depends on the float_precision parameter.

out_flow : ImageView
    {rg16f, rg32f} image. The estimated optical flow.
    The image format depends on the float_precision parameter.

]]

function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = builder.name

    local in_gray = ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView)
    in_gray:checkImageChannelCountIs(ll.ChannelCount.C1)
    in_gray:checkImageChannelTypeIs(ll.ChannelType.Uint8)

    desc:addPort(in_gray)
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:setParameter('alpha', 0.01) -- TODO
    desc:setParameter('iterations', 1)
    desc:setParameter('float_precision', ll.FloatPrecision.FP32)

    return desc
end


function builder.onNodeInit(node)

    ll.logd(node.descriptor.builderName, 'onNodeInit')

    local alpha = node:getParameter('alpha')
    local iterations = node:getParameter('iterations')

    if iterations < 1 then
        error(node.descriptor.builderName .. ': iterations must be greater or equal 1, got: ' .. iterations)
    end
    
    local float_precision = node:getParameter('float_precision')
    local outChannelType = ll.floatPrecisionToImageChannelType(float_precision)

    local in_gray = node:getPort('in_gray')

    local width = in_gray.width
    local height = in_gray.height
    local memory = in_gray.memory

    ll.logd(node.descriptor.builderName, string.format('onNodeInit: input shape: [%d, %d]', width, height))

    local inGrayOldImgDesc = ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C1, outChannelType)
    local inGrayOldImgViewDesc = ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false)
    
    local in_gray_old = memory:createImageView(inGrayOldImgDesc, inGrayOldImgViewDesc)
    in_gray_old:changeImageLayout(ll.ImageLayout.General)

    local copyInputGrayComputeNode = ll.createComputeNode('lluvia/math/normalize/ImageNormalize_uint_C1')
    copyInputGrayComputeNode:bind('in_image_uint', in_gray)
    copyInputGrayComputeNode:bind('out_image_float', in_gray_old)
    copyInputGrayComputeNode:setParameter('max_value', 255)
    copyInputGrayComputeNode:init()
    ll.run(copyInputGrayComputeNode) -- immediately run the node


    local imageProcessor = ll.createComputeNode('lluvia/opticalflow/HornSchunck/ImageProcessor')
    imageProcessor:setParameter('alpha', alpha)
    imageProcessor:setParameter('float_precision', float_precision)
    imageProcessor:bind('in_gray_old', in_gray_old)
    imageProcessor:bind('in_gray', in_gray)
    imageProcessor:init()

    node:bindNode('ImageProcessor', imageProcessor)

    local inImageParms = imageProcessor:getPort('out_image_params')

    -------------------------------------------------------
    -- Numeric iterations to linear system
    -------------------------------------------------------
    local inFlowImgDesc = ll.ImageDescriptor.new(1, height, width, ll.ChannelCount.C2, outChannelType)
    local inFlowImgViewDesc = ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat, ll.ImageFilterMode.Nearest, false, false)

    local inFlow = memory:createImageView(inFlowImgDesc, inFlowImgViewDesc)
    inFlow:changeImageLayout(ll.ImageLayout.General)
    inFlow:clear()

    local outFlow = memory:createImageView(inFlowImgDesc, inFlowImgViewDesc)
    outFlow:changeImageLayout(ll.ImageLayout.General)
    outFlow:clear()

    for i = 1, iterations do

        local numericIteration = ll.createComputeNode('lluvia/opticalflow/HornSchunck/NumericIteration')
        numericIteration:bind('in_image_params', inImageParms)
        numericIteration:bind('in_flow', inFlow)
        numericIteration:bind('out_flow', outFlow)
        numericIteration:init()

        node:bindNode(string.format('NumericIteration_%d', i), numericIteration)

        inFlow = numericIteration:getPort('out_flow')
        outFlow = numericIteration:getPort('in_flow')
    end

    -- close the ring of numeric iterations
    local numericIterationFirst = node:getNode('NumericIteration_1')
    local numericIterationLast = node:getNode('NumericIteration_' .. iterations)
    numericIterationFirst:bind('in_flow', numericIterationLast:getPort('out_flow'))

    -------------------------------------------------------
    
    node:bind('out_flow', numericIterationLast:getPort('out_flow'))
    node:bind('out_gray', imageProcessor:getPort('out_gray'))

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd(node.descriptor.builderName, 'onNodeRecord')

    local iterations = node:getParameter('iterations')

    local imageProcessor = node:getNode('ImageProcessor')
    
    cmdBuffer:run(imageProcessor)
    cmdBuffer:memoryBarrier()

    for i = 1, iterations do
        local numericIteration = node:getNode(string.format('NumericIteration_%d', i))

        cmdBuffer:run(numericIteration)
        cmdBuffer:memoryBarrier()
    end
    
    ll.logd(node.descriptor.builderName, 'onNodeRecord: finish')
end


ll.registerNodeBuilder(builder)
