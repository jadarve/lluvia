local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/camera/CameraRectification_rgba'
builder.doc = [[
Rectifies an RGBA input image applying camera distortion model.

TODO

Inputs
------
in_rgba : ImageView.
    rgba8ui image.

in_camera : UniformBuffer.
    Uniform buffer containing the cameara struct. TODO.

Outputs
-------
out_rgba : ImageView
    rgba8ui image. The rectified image.

]]

function builder.newDescriptor()

    local desc = ll.ComputeNodeDescriptor.new()

    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_rgba = ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.SampledImageView)
    in_rgba:checkImageChannelCountIs(ll.ChannelCount.C4)
    in_rgba:checkImageChannelTypeIs(ll.ChannelType.Uint8)

    local in_camera = ll.PortDescriptor.new(1, 'in_camera', ll.PortDirection.In, ll.PortType.UniformBuffer)

    local out_rgba = ll.PortDescriptor.new(2, 'out_rgba', ll.PortDirection.Out, ll.PortType.ImageView)
    out_rgba:checkImageChannelCountIs(ll.ChannelCount.C4)
    out_rgba:checkImageChannelTypeIs(ll.ChannelType.Uint8)
    
    desc:addPort(in_rgba)
    desc:addPort(in_camera)
    desc:addPort(out_rgba)

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

    -- ll::Memory where out_gray will be allocated
    local memory = in_rgba.memory
    local out_rgba = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_rgba:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_rgba', out_rgba)
    node:configureGridShape(ll.vec3ui.new(out_rgba.width, out_rgba.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder)
