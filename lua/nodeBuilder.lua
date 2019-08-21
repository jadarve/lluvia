local builder = ll.class(ll.ComputeNodeBuilder)

function builder.newDescriptor() 
    
    local desc = ll.ComputeNodeDescriptor.new()
    desc.localShape = ll.vec3ui.new(32, 32, 1)
    desc:configureGridShape(ll.vec3ui.new(1920, 1080, 1))
    
    -- desc.program = ll.getProgram('programName')
    desc.functionName = 'main'

    desc:addPort(ll.PortDescriptor.new(0, 'in_buffer', ll.PortDirection.In, ll.PortType.Buffer))
    desc:addPort(ll.PortDescriptor.new(1, 'out_buffer', ll.PortDirection.Out, ll.PortType.Buffer))

    return desc
end

function builder.onNodeInit(node)
    print('builder.onNodeInit')
end

ll.registerNodeBuilder('moniBuilder', builder)

b = ll.getNodeBuilder('moniBuilder')

desc = b.newDescriptor()
print(desc.functionName)
print(desc.localShape.x)
print(desc.gridShape.x)
