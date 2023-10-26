local builder = ll.class(ll.ContainerNodeBuilder)

builder.name = 'lluvia/opticalflow/HornSchunck/HornSchunck'
builder.doc = [[
Horn and Schunck variational optical flow algorithm.

Parameters
----------
alpha : float. Defaults to 0.05.
    Regularization gain.

iterations : int. Defaults to 1.
    Number of iterations run to compute the optical flow.

float_precision : int. Defaults to ll.FloatPrecision.FP32.
    Floating point precision used accross the algorithm. The outputs out_gray
    and out_flow will be of this floating point precision.

clear_history : int. Defaults to 1.
    If set to 1, the history of the optical flow is cleared before running the
    algorithm on the next input image.

Inputs
------
in_gray : ImageView
    r8ui image. The input gray-scale image. It should contain valid image data before
    initializing the node in order to avoid residual flow calculcations in the first
    iterations of the algorithm.

Outputs
-------
out_gray : ImageView
    {r16f, r32f} image. The gray-scale image after one iteration of the algorithm.
    The image format depends on the float_precision parameter.

out_flow : ImageView
    {rg16f, rg32f} image. The estimated optical flow.
    The image format depends on the float_precision parameter.


Examples
--------

```python
import lluvia as ll
import lluvia.util as ll_util
import matplotlib.pyplot as plt

# read two images as numpy arrays
frame_0 = ll_util.readRGBA('path to first image...')
frame_1 = ll_util.readRGBA('path to second image...')

# global session and memory objects
session = ll.createSession()
memory = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

# this is the input of the comple pipeline
in_rgba = memory.createImageViewFromHost(frame_0)

RGBA2Gray = session.createComputeNode('lluvia/color/RGBA2Gray')
RGBA2Gray.bind('in_rgba', in_rgba)
RGBA2Gray.init()
RGBA2Gray.run() # run the node immediately in order to populate out_gray with valid values

in_gray = RGBA2Gray.getPort('out_gray')

HornSchunck = session.createContainerNode('lluvia/opticalflow/HornSchunck/HornSchunck')
HornSchunck.setParameter('alpha', ll.Parameter(0.05))
HornSchunck.setParameter('iterations', ll.Parameter(1000))
HornSchunck.setParameter('float_precision', ll.Parameter(ll.FloatPrecision.FP32.value))
HornSchunck.bind('in_gray', in_gray)

# when the node is initialized, it transfers the content of in_gray to out_gray.
HornSchunck.init()

out_gray = HornSchunck.getPort('out_gray')
out_flow = HornSchunck.getPort('out_flow')

flow2RGBA = session.createComputeNode('lluvia/viz/Flow2RGBA')
flow2RGBA.setParameter('max_flow', ll.Parameter(float(2)))
flow2RGBA.bind('in_flow', out_flow)
flow2RGBA.init()

out_flow_rgba = flow2RGBA.getPort('out_rgba')

duration = session.createDuration()

cmdBuffer = session.createCommandBuffer()
cmdBuffer.begin()
cmdBuffer.run(RGBA2Gray)
cmdBuffer.memoryBarrier()
cmdBuffer.durationStart(duration)
cmdBuffer.run(HornSchunck)
cmdBuffer.memoryBarrier()
cmdBuffer.durationEnd(duration)
cmdBuffer.run(flow2RGBA)
cmdBuffer.end()

# copy the content of the second frame to the in_rgba image before running the whole pipeline
in_rgba.fromHost(frame_1)

# run the pipeline
session.run(cmdBuffer)

# print runtime in milliseconds
print('{0:.02f} ms'.format(duration.nanoseconds / 1e6))

fig = plt.figure(figsize=(10, 6)); fig.set_tight_layout(True)
plt.subplot2grid((1,2), (0, 0)); plt.imshow(out_gray.toHost(), vmin=0, vmax=1, cmap='gray')
plt.subplot2grid((1,2), (0, 1)); plt.imshow(out_flow_rgba.toHost())
plt.show()
```

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

    -- Parameters
    desc:setParameter('alpha', 0.05)
    desc:setParameter('iterations', 1)
    desc:setParameter('float_precision', ll.FloatPrecision.FP32)
    desc:setParameter('clear_history', 1)

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

    ---------------------------------------------------------------------------
    -- Copy of in_gray to in_gray_old. This runs after the ImageProcessor and
    -- copies + normalizes in_gray and stores it into in_gray_old/out_gray.
    --
    -- This is needed as the ImageProcessor shader reads a neighborhood of 
    -- pixels in in_gray_old, hence it is not possible to write the new
    -- in_gray_old value as part of the shader, as it will create a race
    -- condition with neighboring pixel executions.
    ---------------------------------------------------------------------------
    local copyInGrayToInGrayOld = ll.createComputeNode('lluvia/math/normalize/ImageNormalize_uint_C1')
    copyInGrayToInGrayOld:bind('in_image_uint', in_gray)
    copyInGrayToInGrayOld:bind('out_image_float', in_gray_old)
    copyInGrayToInGrayOld:setParameter('max_value', 255)
    copyInGrayToInGrayOld:init()

    -- Immediately run the node as part of the initialization sequence.
    -- This enables the HornSchunck node to compute good optical flow in the first run.
    ll.run(copyInGrayToInGrayOld)

    node:bindNode('CopyInGrayToInGrayOld', copyInGrayToInGrayOld)

    ---------------------------------------------------------------------------
    -- Image processor
    ---------------------------------------------------------------------------
    local imageProcessor = ll.createComputeNode('lluvia/opticalflow/HornSchunck/ImageProcessor')
    imageProcessor:setParameter('alpha', alpha)
    imageProcessor:setParameter('float_precision', float_precision)
    imageProcessor:bind('in_gray_old', in_gray_old)
    imageProcessor:bind('in_gray', in_gray)
    imageProcessor:init()

    node:bindNode('ImageProcessor', imageProcessor)

    local inImageParms = imageProcessor:getPort('out_image_params')

    ---------------------------------------------------------------------------
    -- Numeric iterations to linear system
    ---------------------------------------------------------------------------
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
    local numericIterationLast = node:getNode(string.format('NumericIteration_%d', iterations))
    numericIterationFirst:bind('in_flow', numericIterationLast:getPort('out_flow'))

    ---------------------------------------------------------------------------
    
    node:bind('out_flow', numericIterationLast:getPort('out_flow'))
    node:bind('out_gray', in_gray_old)

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd(node.descriptor.builderName, 'onNodeRecord')

    local iterations = node:getParameter('iterations')
    local clear_history = node:getParameter('clear_history')

    local imageProcessor = node:getNode('ImageProcessor')
    local copyInGrayToInGrayOld = node:getNode('CopyInGrayToInGrayOld')
    
    -- first run the image processor, then copy the content of in_gray into in_gray_old
    cmdBuffer:run(imageProcessor)
    cmdBuffer:memoryBarrier()
    cmdBuffer:run(copyInGrayToInGrayOld)
    cmdBuffer:memoryBarrier()

    -- check if the history of the optical flow should be cleared before the numeric iterations
    if clear_history > 0 then
        local out_flow = node:getPort('out_flow')
        cmdBuffer:clearImage(out_flow)
    end

    for i = 1, iterations do
        local numericIteration = node:getNode(string.format('NumericIteration_%d', i))

        cmdBuffer:run(numericIteration)
        cmdBuffer:memoryBarrier()
    end
    
    ll.logd(node.descriptor.builderName, 'onNodeRecord: finish')
end


ll.registerNodeBuilder(builder)
