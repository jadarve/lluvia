local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/imgproc/ImageDownsampleX_r8ui'
builder.doc = [[
Downsamples a gray level image along the X axis.

Let W and H denote the width and height of the input in_gray image, respectively.
The shape of the out_gray image is:

* out_gray.W = floor(W / 2)
* out_gray.H = H

That is, the output width is always an even number.

The value of a pixel out_gray(x, y) is computed as:

out_gray(x, y) = 0.25*in_gray(2*x - 1, y) + 0.5*in_gray(2*x, y) + 0.25*in_gray(2*x + 1, y)

Inputs
------
in_gray : ImageView.
    r8ui image.

Outputs
-------
out_gray : ImageView
    r8ui image. The downsampled image.

]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()

    desc:init(builder.name, ll.ComputeDimension.D2)
    
    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    local in_gray = node:getPort('in_gray')

    -- validate in_rgba is actually a rgba8ui image
    -- TODO: remove once port-contracts are implemented
    local err = ll.isValidImage(in_gray, ll.ChannelCount.C1, ll.ChannelType.Uint8)
    if err ~= nil then
        error(builder.name .. ': error validating in_gray: ' .. err)
    end

    -- out_gray descriptors
    local imgDesc = ll.ImageDescriptor.new(in_gray.imageDescriptor)
    imgDesc.width = in_gray.width // 2

    local imgViewDesc = ll.ImageViewDescriptor.new()
    imgViewDesc.filterMode = ll.ImageFilterMode.Nearest
    imgViewDesc.normalizedCoordinates = false
    imgViewDesc.isSampled = false
    imgViewDesc:setAddressMode(ll.ImageAddressMode.Repeat)

    -- ll::Memory where out_gray will be allocated
    local memory = in_gray.memory
    local out_gray = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_gray:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gray', out_gray)

    ll.logd(builder.name, 'in_gray ', string.format('[%d, %d, %d]', in_gray.width, in_gray.height, in_gray.channelCount)
                              , 'out_gray', string.format('[%d, %d, %d]', out_gray.width, out_gray.height, out_gray.channelCount))

    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder.name, builder)
