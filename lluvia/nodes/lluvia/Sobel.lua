local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/Sobel'
builder.doc = [[
Process a gray image with the Sobel gradient filter.

The in_gray image is filtered with the Sobel masks:

         [-1/4    0    1/4]
    Sx = [-1/2    0    1/2]
         [-1/4    0    1/4]
        
         [-1/4  -1/2  -1/4]
    Sy = [  0     0     0 ]
         [1/4    1/2   1/4]

The result out_gradient is computed as:

    out_gradient.x = in_gray * Sx
    out_gradient.y = in_gray * Sy

The border is clamped to the edges of the image.

Inputs
------
in_gray : ImageView
    r8ui image.

Outputs
-------
out_gradient : ImageView
    rg32f image. The image gradient.

]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'Sobel'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('Sobel')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'out_gradient', ll.PortDirection.Out, ll.PortType.ImageView))

    return desc
end

function builder.onNodeInit(node)

    local in_gray = node:getPort('in_gray')
    local memory = in_gray.memory

    local gradientImgDesc = ll.ImageDescriptor.new(1,
                                                   in_gray.height,
                                                   in_gray.width,
                                                   ll.ChannelCount.C2,
                                                   ll.ChannelType.Float32)

    -- normalizedCoordinates : false
    -- isSampled             : false
    local imgViewDesc = ll.ImageViewDescriptor.new(ll.ImageAddressMode.MirroredRepeat,
                                                   ll.ImageFilterMode.Nearest,
                                                   false,
                                                   false)

    -- memory allocation
    local out_gradient = memory:createImageView(gradientImgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_gradient:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_gradient', out_gradient)
    node:configureGridShape(ll.vec3ui.new(out_gray.width, out_gray.height, 1))
end

ll.registerNodeBuilder(builder.name, builder)
