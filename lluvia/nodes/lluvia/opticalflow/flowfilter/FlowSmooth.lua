local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/FlowSmooth'
builder.doc = [[
Smooths an optical flow field using a 3x3 Gaussian filter

Parameters
----------
allocate_output : int. Defaults to 1.
    Whether or not the out_flow output should be allocated. If zero,
    this node does not allocate any output and expects out_flow to be
    allocated and bound before this node is initialized.

Inputs
------
in_flow : ImageView
    {rg16f, rg32f} image. The input optical flow.

Outputs
-------
out_flow : ImageView
    {rg16f, rg32f} image. Smoothed optical flow. The floating point precision
    will be the same as in_flow.

]]

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_flow = ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView)
    in_flow:checkImageChannelCountIs(ll.ChannelCount.C2)
    in_flow:checkImageChannelTypeIsAnyOf({ll.ChannelType.Float16, ll.ChannelType.Float32})

    desc:addPort(in_flow)
    desc:addPort(ll.PortDescriptor.new(1, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- whether or not the out_flow should be allocated, defaults to true
    desc:setParameter('allocate_output', 1)

    return desc
end

function builder.onNodeInit(node)

    local allocate_output = node:getParameter('allocate_output')

    ll.logd(node.descriptor.builderName, 'onNodeInit', 'start, allocate_output:', allocate_output)

    local in_flow = node:getPort('in_flow')

    local out_flow = nil
    if allocate_output ~= 0 then

        ll.logd(node.descriptor.builderName, 'onNodeInit: allocating output')
        
        local memory = in_flow.memory
        out_flow = memory:createImageView(in_flow.imageDescriptor, in_flow.descriptor)
        out_flow:changeImageLayout(ll.ImageLayout.General)
        node:bind('out_flow', out_flow)

    else

        out_flow = node:getPort('out_flow')
    end

    
    node:configureGridShape(ll.vec3ui.new(out_flow.width, out_flow.height, 1))

    -- clear outputs
    out_flow:clear()

    ll.logd(node.descriptor.builderName, 'onNodeInit', 'finish')
end

ll.registerNodeBuilder(builder)
