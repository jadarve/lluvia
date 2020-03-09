local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'FlowSmooth'
    desc.localShape   = ll.vec3ui.new(32, 32, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('FlowSmooth')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_flow', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- whether or not the out_flow should be allocated, defaults to true
    desc:setParameter('allocate_output', 1)

    return desc
end

function builder.onNodeInit(node)

    allocate_output = node:getParameter('allocate_output')

    ll.logd('FlowSmooth', 'onNodeInit', 'start, allocate_output:', allocate_output)

    in_flow = node:getPort('in_flow')

    if allocate_output ~= 0 then

        ll.logd('FlowSmooth', 'onNodeInit: allocating output')
        
        memory = in_flow.memory
        out_flow = memory:createImageView(in_flow.imageDescriptor, in_flow.descriptor)
        out_flow:changeImageLayout(ll.ImageLayout.General)
        node:bind('out_flow', out_flow)
    end

    node:configureGridShape(ll.vec3ui.new(out_flow.width, out_flow.height, 1))

    -- clear outputs
    node:getPort('out_flow'):clear()

    ll.logd('FlowSmooth', 'onNodeInit', 'finish')
end

ll.registerNodeBuilder('FlowSmooth', builder)
