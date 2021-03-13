local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/color/RGBA2Gray'
builder.doc = [[

RGB to Gray
===========

Converts a RGB image to gray scale. The gray scale value is computed as:

	gray = dot(RGBA, vec4(0.29899999, 0.58700001, 0.114, 0.0))    

Inputs
------
 
* **in_rgba:** rgba8ui ImageView.


Outputs
-------

* **out_gray:** r8ui ImageView
    
    The output gray scale image in range [0, 255]. The output is allocated in the
    same memory as in_rgba

]]

function builder.newDescriptor()

    local desc = ll.ComputeNodeDescriptor.new()

    desc:init(builder.name, ll.ComputeDimension.D2)

    desc:addPort(ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    local in_rgba = node:getPort('in_rgba')

    -- validate in_rgba is actually a rgba8ui image
    local err = ll.isValidImage(in_rgba, ll.ChannelCount.C4, ll.ChannelType.Uint8)
    if err ~= nil then
        error(builder.name .. ': error validating in_rgba: ' .. err)
    end
    
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
    imgDesc.channelCount = 1
    imgDesc.channelType = ll.ChannelType.Uint8

    local imgViewDesc = ll.ImageViewDescriptor.new()
    imgViewDesc.filterMode = ll.ImageFilterMode.Nearest
    imgViewDesc.normalizedCoordinates = false
    imgViewDesc.isSampled = false
    imgViewDesc:setAddressMode(ll.ImageAddressMode.Repeat)

    -- ll::Memory where out_gray will be allocated
    local memory = in_rgba.memory
    local out_gray = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_gray:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gray', out_gray)
    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder.name, builder)
