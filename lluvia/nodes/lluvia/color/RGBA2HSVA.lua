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
float_precision : int. Defaults to 32.
    Floating point precision used to alloate out_hsva. Possible values: [16, 32].

Inputs
------
in_rgba : ImageView.
    rgba8ui image.

Outputs
-------
out_hsva : ImageView
    rgba32f image. This image is allocated in the same memory as in_rgba.
    
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
    desc:setParameter('float_precision', 32)

    -- TOTHINK: increased port contracts by checking internal attributes of the PortType
    -- For ImageView, check all image attributes + image view attributes.
    desc:addPort(ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_hsva', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

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
    -- validate input
    -------------------------------------------------------
    local in_rgba = node:getPort('in_rgba')

    -- validate in_rgba is actually a rgba8ui image
    -- TODO: remove once port-contracts are implemented
    local err = ll.isValidImage(in_rgba, ll.ChannelCount.C4, ll.ChannelType.Uint8)
    if err ~= nil then
        error(builder.name .. ': error validating in_rgba: ' .. err)
    end
    
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

    -- factor out in a function
    if float_precision == 16 then
        imgDesc.channelType = ll.ChannelType.Float16
    elseif float_precision == 32 then
        imgDesc.channelType = ll.ChannelType.Float32
    else
        error(builder.name .. ': unknown float_precision got: ' .. float_precision)
    end

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
