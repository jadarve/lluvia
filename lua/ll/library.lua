
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


function ll.castObject(obj)

    castTable = {
        [ll.ObjectType.Buffer] = ll.impl.castBuffer,
        [ll.ObjectType.Image] = ll.impl.castImage,
        [ll.ObjectType.ImageView] = ll.impl.castImageView
    }
    return castTable[obj.type](obj)
end


ll.ComputeNodeBuilder = ll.class()

function ll.ComputeNodeBuilder.newDescriptor()
    error('newDescriptor must be implemented by child classes')
end

function ll.ComputeNodeBuilder.onNodeInit(node)
    print('ll.ComputeNodeBuilder.onNodeInit')
    -- do nothing
end
