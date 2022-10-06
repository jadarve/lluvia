local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/color/RGBA2BGRA'
builder.doc = [[
Converts a RGBA image to BGRA.

Inputs
------
in_rgba : ImageView.
    rgba8ui image in RGBA channel order.

Outputs
-------
out_bgra : ImageView.
    rgba8ui image in BGRA channel order.

]]

function builder.newDescriptor()

    local desc = ll.ComputeNodeDescriptor.new()

    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_rgba = ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView)
    in_rgba:checkImageChannelCountIs(ll.ChannelCount.C4)
    in_rgba:checkImageChannelTypeIs(ll.ChannelType.Uint8)
    desc:addPort(in_rgba)
    
    desc:addPort(ll.PortDescriptor.new(1, 'out_bgra', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    local in_rgba = node:getPort('in_rgba')
    
    -------------------------------------------------------
    -- allocate out_rgba
    -------------------------------------------------------
    local width = in_rgba.width
    local height = in_rgba.height
    local depth = in_rgba.depth

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

    -- ll::Memory where out_bgra will be allocated
    local memory = in_rgba.memory
    local out_bgra = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_bgra:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_bgra', out_bgra)
    node:configureGridShape(ll.vec3ui.new(out_bgra.width, out_bgra.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder)
