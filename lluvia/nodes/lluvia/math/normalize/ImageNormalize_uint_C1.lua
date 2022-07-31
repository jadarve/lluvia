local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/math/normalize/ImageNormalize_uint_C1'
builder.doc = [[
Normalizes the content of a 1-channel unsigned integer image and stores it into a 1-channel floating point image.

Parameters
----------
max_value : float. Defaults to 0.0.
    If the value is greater than 0. The input value is divided by max_value and then
    stored in out_image_float. Otherwise, the input value is casted to floating point and stored directly.

Inputs
------
in_image_uint : ImageView.
    {r8ui, r16ui, r32ui} image.

Outputs
-------
out_image_float : ImageView.
    {r16f, r32f} image. This image must be allocated externally.

Examples
--------

```python
import lluvia as ll
import lluvia.util as ll_util
import matplotlib.pyplot as plt

# global session and memory objects
session = ll.createSession()
memory = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

# read a sample image
sampleImage = ll_util.readSampleImage('mouse')

# this is the input of the comple pipeline
in_rgba = memory.createImageViewFromHost(sampleImage)

RGBA2Gray = session.createComputeNode('lluvia/color/RGBA2Gray')
RGBA2Gray.bind('in_rgba', in_rgba)
RGBA2Gray.init()

# input and output to ImageNormalize. out_gray_float must be allocated outside of the node
out_gray_uint = RGBA2Gray.getPort('out_gray')
out_gray_float = memory.createImage(out_gray_uint.shape, ll.ChannelType.Float32).createImageView()

ImageNormalize = session.createComputeNode('lluvia/math/normalize/ImageNormalize_uint_C1')
ImageNormalize.setParameter('max_value', ll.Parameter(255))
ImageNormalize.bind('in_image_uint', out_gray_uint)
ImageNormalize.bind('out_image_float', out_gray_float)
ImageNormalize.init()

duration = session.createDuration()

cmdBuffer = session.createCommandBuffer()
cmdBuffer.begin()
cmdBuffer.run(RGBA2Gray)
cmdBuffer.memoryBarrier()
cmdBuffer.durationStart(duration)
cmdBuffer.run(ImageNormalize)
cmdBuffer.memoryBarrier()
cmdBuffer.durationEnd(duration)
cmdBuffer.end()

# run the pipeline
session.run(cmdBuffer)

# print runtime in milliseconds
print('{0:.04f} ms'.format(duration.nanoseconds / 1e6))

fig = plt.figure(figsize=(30, 6)); fig.set_tight_layout(True)
plt.subplot2grid((1, 3), (0, 0)); plt.imshow(in_rgba.toHost()[...,:3])
plt.subplot2grid((1, 3), (0, 1)); plt.imshow(out_gray_uint.toHost(), vmin=0, vmax=255); plt.colorbar()
plt.subplot2grid((1, 3), (0, 2)); plt.imshow(out_gray_float.toHost(), vmin=0, vmax=1); plt.colorbar()
plt.show()
```

]]

function builder.newDescriptor()

    local desc = ll.ComputeNodeDescriptor.new()

    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_image_uint = ll.PortDescriptor.new(0, 'in_image_uint', ll.PortDirection.In, ll.PortType.ImageView)
    in_image_uint:checkImageChannelCountIs(ll.ChannelCount.C1)
    in_image_uint:checkImageChannelTypeIsAnyOf({ll.ChannelType.Uint8, ll.ChannelType.Uint16, ll.ChannelType.Uint32})

    local out_image_float = ll.PortDescriptor.new(1, 'out_image_float', ll.PortDirection.Out, ll.PortType.ImageView)
    out_image_float:checkImageChannelCountIs(ll.ChannelCount.C1)
    out_image_float:checkImageChannelTypeIsAnyOf({ll.ChannelType.Float16, ll.ChannelType.Float32})

    desc:addPort(in_image_uint)
    desc:addPort(out_image_float)

    desc:setParameter('max_value', 0.0)

    return desc
end

function builder.onNodeInit(node)

    local out_image_float = node:getPort('out_image_float')

    local max_value = node:getParameter('max_value')

    local pushConstants = ll.PushConstants.new()
    pushConstants:pushFloat(max_value)

    node.pushConstants = pushConstants

    node:configureGridShape(ll.vec3ui.new(out_image_float.width, out_image_float.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder)
