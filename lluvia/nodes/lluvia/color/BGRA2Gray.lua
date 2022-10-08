local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/color/BGRA2Gray'
builder.doc = [[
Converts a BGRA image to gray scale.

The gray scale value is computed as:

	gray = dot(RGBA, vec4(0.29899999, 0.58700001, 0.114, 0.0))

where `RGBA` is read and ordered from the in_bgra input image. 

Inputs
------
in_bgra : ImageView.
    rgba8ui image in BGRA channel order.

Outputs
-------
out_gray : ImageView
    r8ui image. Thre gray scale values are in the range [0, 255]. This image is allocated in the
    same memory as in_bgra.

]]

function builder.newDescriptor()

    local desc = ll.ComputeNodeDescriptor.new()

    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_bgra = ll.PortDescriptor.new(0, 'in_bgra', ll.PortDirection.In, ll.PortType.ImageView)
    in_bgra:checkImageChannelCountIs(ll.ChannelCount.C4)
    in_bgra:checkImageChannelTypeIs(ll.ChannelType.Uint8)
    desc:addPort(in_bgra)
    
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    local in_bgra = node:getPort('in_bgra')
    
    -------------------------------------------------------
    -- allocate out_rgba
    -------------------------------------------------------
    local width = in_bgra.width
    local height = in_bgra.height
    local depth = in_bgra.depth

    local imgDesc = ll.ImageDescriptor.new()
    imgDesc.width = width
    imgDesc.height = height
    imgDesc.depth = depth
    imgDesc.channelCount = ll.ChannelCount.C1
    imgDesc.channelType = ll.ChannelType.Uint8

    local imgViewDesc = ll.ImageViewDescriptor.new()
    imgViewDesc.filterMode = ll.ImageFilterMode.Nearest
    imgViewDesc.normalizedCoordinates = false
    imgViewDesc.isSampled = false
    imgViewDesc:setAddressMode(ll.ImageAddressMode.Repeat)

    -- ll::Memory where out_gray will be allocated
    local memory = in_bgra.memory
    local out_gray = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_gray:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gray', out_gray)
    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder)
