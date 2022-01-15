local builder = ll.class(ll.ComputeNodeBuilder)
builder.name = 'nodes/Assign'

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    
    desc.builderName  = 'nodes/Assign'
    desc.localShape   = ll.vec3ui.new(32, 1, 1)
    desc.gridShape    = ll.vec3ui.new(1, 1, 1)
    desc.program      = ll.getProgram('nodes/Assign.comp')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'out_buffer', ll.PortDirection.Out, ll.PortType.Buffer))

    return desc
end

function builder.onNodeInit(node)

    local out_buffer = node:getPort('out_buffer')
    local N = out_buffer // 4

    node:configureGridShape(ll.vec3ui.new(N, 1, 1))
end

ll.registerNodeBuilder(builder)
