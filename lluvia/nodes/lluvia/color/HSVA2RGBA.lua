local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/color/HSVA2RGBA'
builder.doc = [[
Converts a HSV image to RGB color space.

The conversion follows the formulae presented in https://en.wikipedia.org/wiki/HSL_and_HSV.

Inputs
------
in_hsva : ImageView.
    rgba32f image. The color componens must lie within the following ranges:

    * H in [0, 2*pi]
    * S in [0, 1]
    * V in [0, 1]
    * A in [0, 1]

Outputs
-------
out_rgba : ImageView
    rgba8ui image. This image is allocated in the same memory as in_hsva.
    
    
]]

function builder.newDescriptor()

    local desc = ll.ComputeNodeDescriptor.new()

    desc:init(builder.name, ll.ComputeDimension.D2)

    -- TOTHINK: increased port contracts by checking internal attributes of the PortType
    -- For ImageView, check all image attributes + image view attributes.
    desc:addPort(ll.PortDescriptor.new(0, 'in_hsva', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_rgba', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    -------------------------------------------------------
    -- validate input
    -------------------------------------------------------
    local in_hsva = node:getPort('in_hsva')

    -- validate in_hsva is actually a rgba8ui image
    -- TODO: remove once port-contracts are implemented
    local err = ll.isValidImage(in_hsva, ll.ChannelCount.C4, ll.ChannelType.Float32)
    if err ~= nil then
        error(builder.name .. ': error validating in_hsva: ' .. err)
    end
    
    -------------------------------------------------------
    -- allocate out_rgba
    -------------------------------------------------------
    local width = in_hsva.width
    local height = in_hsva.height
    local depth = in_hsva.depth

    local imgDesc = ll.ImageDescriptor.new()
    imgDesc.width = width
    imgDesc.height = height
    imgDesc.depth = depth
    imgDesc.channelCount = ll.ChannelCount.C4
    imgDesc.channelType = ll.ChannelType.Uint8

    local imgViewDesc = ll.ImageViewDescriptor.new()
    imgViewDesc.filterMode = ll.ImageFilterMode.Nearest
    imgViewDesc.normalizedCoordinates = false
    imgViewDesc.isSampled = false
    imgViewDesc:setAddressMode(ll.ImageAddressMode.Repeat)

    -- ll::Memory where out_rgba will be allocated
    local memory = in_hsva.memory
    local out_rgba = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_rgba:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_rgba', out_rgba)
    node:configureGridShape(ll.vec3ui.new(out_rgba.width, out_rgba.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder.name, builder)
