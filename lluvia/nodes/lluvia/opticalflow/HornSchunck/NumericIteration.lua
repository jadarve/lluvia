local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/opticalflow/HornSchunck/NumericIteration'
builder.doc = [[
Computes one iteration for solving the system of equations to compute optical flow.

TODO

Inputs
------
in_image_params : ImageView.
    {rgba16f, rgba32f} image. TODO

in_flow: ImageView.
    {rg16f, rg32f} image.

Outputs
-------
out_flow: ImageView
    {rg16f, rg32f} image. TODO

]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_image_params = ll.PortDescriptor.new(0, 'in_image_params', ll.PortDirection.In, ll.PortType.ImageView)
    in_image_params:checkImageChannelCountIs(ll.ChannelCount.C4)
    in_image_params:checkImageChannelTypeIsAnyOf({ll.ChannelType.Float16, ll.ChannelType.Float32})

    local in_flow = ll.PortDescriptor.new(1, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView)
    in_flow:checkImageChannelCountIs(ll.ChannelCount.C2)
    in_flow:checkImageChannelTypeIsAnyOf({ll.ChannelType.Float16, ll.ChannelType.Float32})

    local out_flow = ll.PortDescriptor.new(2, 'out_flow', ll.PortDirection.In, ll.PortType.ImageView)
    out_flow:checkImageChannelCountIs(ll.ChannelCount.C2)
    out_flow:checkImageChannelTypeIsAnyOf({ll.ChannelType.Float16, ll.ChannelType.Float32})

    desc:addPort(in_image_params)
    desc:addPort(in_flow)
    desc:addPort(out_flow)
    
    return desc
end

function builder.onNodeInit(node)
    ll.logd(node.descriptor.builderName, 'onNodeInit')

    local in_image_params = node:getPort('in_image_params')

    node:configureGridShape(ll.vec3ui.new(in_image_params.width, in_image_params.height, 1))

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')
end

ll.registerNodeBuilder(builder)
