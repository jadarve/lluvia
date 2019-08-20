
ll['nodeBuilders'] = {}

function ll.class(base)

    local c = {}

    if base then

        -- shallow copy of base class members
        if type(base) == 'table' then
            for i, v in pairs(base) do
                c[i] = v
            end
        end
    end

    c.__index = c

    return c
end


function ll.registerNodeBuilder(name, builder)
    ll.nodeBuilders[name] = builder
end


function ll.getNodeBuilder(name)
    return ll.nodeBuilders[name]
end



ll.ComputeNodeBuilder = ll.class()

function ll.ComputeNodeBuilder.newDescriptor()
    error('newDescriptor must be implemented by child classes')
end

function ll.ComputeNodeBuilder.onNodeInit(node)
    print('ll.ComputeNodeBuilder.onNodeInit')
    -- do nothing
end



-- This might be in another file

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
