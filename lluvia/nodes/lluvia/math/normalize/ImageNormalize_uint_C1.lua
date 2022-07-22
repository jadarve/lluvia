local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/math/normalize/ImageNormalize_uint_C1'
builder.doc = [[
Normalized the content of a 1-channel unsigned integer image and stores it into a 1-channel floating point image.

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
