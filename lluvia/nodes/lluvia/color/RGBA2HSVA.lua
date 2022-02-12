local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/color/RGBA2HSVA'
builder.doc = [[
Converts a RGB image to HSV color space.

The conversion follows the formulae presented in https://en.wikipedia.org/wiki/HSL_and_HSV.

Parameters
----------
min_chroma : float in [0, 1]. Defaults to 0.0.
    The minimum chromacity allowed in the conversion. If the chromacity of a given
    pixel is less than min_chroma, then the hue value is set to 0.
float_precision : int. Defaults to ll.FloatPrecision.FP32.
    Floating point precision used to alloate out_hsva.

Inputs
------
in_rgba : ImageView.
    rgba8ui image.

Outputs
-------
out_hsva : ImageView
    {rgba16f, rgba32f} image. This image is allocated in the same memory as in_rgba.
    
    The color componens lie within the following ranges:

    * H in [0, 2*pi]
    * S in [0, 1]
    * V in [0, 1]
    * A in [0, 1]
]]

function builder.newDescriptor()

    local desc = ll.ComputeNodeDescriptor.new()

    desc:init(builder.name, ll.ComputeDimension.D2)

    desc:setParameter('min_chroma', 0.0)
    desc:setParameter('float_precision', ll.FloatPrecision.FP32)

    local in_rgba = ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView)
    in_rgba:checkImageChannelCountIs(ll.ChannelCount.C4)
    in_rgba:checkImageChannelTypeIs(ll.ChannelType.Uint8)

    desc:addPort(in_rgba)
    desc:addPort(ll.PortDescriptor.new(1, 'out_hsva', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    local in_rgba = node:getPort('in_rgba')

    local float_precision = node:getParameter('float_precision')

    -------------------------------------------------------
    -- push constants
    -------------------------------------------------------
    local min_chroma = node:getParameter('min_chroma')

    if min_chroma < 0 or min_chroma > 1 then
        error(builder.name .. ': min_chroma must be in range [0, 1], got: ' .. min_chroma)
    end

    local pushConstants = ll.PushConstants.new()
    pushConstants.float = min_chroma
    node.pushConstants = pushConstants

    -------------------------------------------------------
    -- allocate out_hsva
    -------------------------------------------------------
    local width = in_rgba.width
    local height = in_rgba.height
    local depth = in_rgba.depth

    local imgDesc = ll.ImageDescriptor.new()
    imgDesc.width = width
    imgDesc.height = height
    imgDesc.depth = depth
    imgDesc.channelCount = ll.ChannelCount.C4
    imgDesc.channelType = ll.floatPrecisionToImageChannelType(float_precision)

    local imgViewDesc = ll.ImageViewDescriptor.new()
    imgViewDesc.filterMode = ll.ImageFilterMode.Nearest
    imgViewDesc.normalizedCoordinates = false
    imgViewDesc.isSampled = false
    imgViewDesc:setAddressMode(ll.ImageAddressMode.Repeat)

    -- ll::Memory where out_hsva will be allocated
    local memory = in_rgba.memory
    local out_hsva = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_hsva:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_hsva', out_hsva)
    node:configureGridShape(ll.vec3ui.new(out_hsva.width, out_hsva.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder)
