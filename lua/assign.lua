local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName = 'assign'
    desc.localShape = ll.vec3ui.new(32, 1, 1)
    desc.gridShape = ll.vec3ui.new(1, 1, 1)
    
    -- desc.program = ll.getProgram('assign')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'out_buffer', ll.PortDirection.Out, ll.PortType.Buffer))

    return desc
end

function builder.onNodeInit(node)

    print('assign.onNodeInit')

    -- configure gridShape given the size of out_buffer
    -- out_buffer = node.getPort('out_buffer')
    -- node.configureGridShape(ll.vec3ui.new(N, 1, 1))

    -- I can also bind objects here
    print('assign.onNodeInit : finish')
end

ll.registerNodeBuilder('assign', builder)
