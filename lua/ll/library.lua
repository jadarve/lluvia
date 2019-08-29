
ll['nodeBuilders'] = {}
ll['activeSession'] = nil


function ll.logd(tag, ...)
    print(tag, ...)
end


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
        [ll.ObjectType.Buffer]    = ll.impl.castObjectToBuffer,
        [ll.ObjectType.Image]     = ll.impl.castObjectToImage,
        [ll.ObjectType.ImageView] = ll.impl.castObjectToImageView
    }

    return castTable[obj.type](obj)
end


function ll.castNode(node)

    castTable = {
        [ll.NodeType.Compute]    = ll.impl.castNodeToComputeNode,
        [ll.NodeType.Container]  = ll.impl.castNodeToContainerNode
    }

    return castTable[node.type](node)
end


function ll.getProgram(name)
    
    if not ll.activeSession then
        error('ll.activeSession nil')
    end

    return ll.activeSession:getProgram(name)
end


-----------------------------------------------------------
--                 ComputeNodeBuilder
-----------------------------------------------------------
ll.ComputeNodeBuilder = ll.class()

function ll.ComputeNodeBuilder.newDescriptor()
    error('newDescriptor must be implemented by child classes')
end

function ll.ComputeNodeBuilder.onNodeInit(node)
    -- do nothing
end


-----------------------------------------------------------
--                 ContainerNodeBuilder
-----------------------------------------------------------
ll.ContainerNodeBuilder = ll.class()

function ll.ContainerNodeBuilder.newDescriptor()
    error('newDescriptor must be implemented by child classes')
end

function ll.ContainerNodeBuilder.onNodeInit(node)
    -- do nothing
end

function ll.ContainerNodeBuilder.onNodeRecord(node, cmdBuffer)
    -- do nothing
end


-----------------------------------------------------------
--                     ComputeNode
-----------------------------------------------------------
function ll.ComputeNode:getPort(name)
    return ll.castObject(self:__getPort(name))
end


function ll.ComputeNode:bind(name, obj)

    castTable = {
        [ll.ObjectType.Buffer]    = ll.impl.castBufferToObject,
        [ll.ObjectType.Image]     = ll.impl.castImageToObject,
        [ll.ObjectType.ImageView] = ll.impl.castImageViewToObject
    }

    self:__bind(name, castTable[obj.type](obj))
end


-----------------------------------------------------------
--                     ContainerNode
-----------------------------------------------------------
function ll.ContainerNode:getPort(name)
    return ll.castObject(self:__getPort(name))
end


function ll.ContainerNode:bind(name, obj)

    castTable = {
        [ll.ObjectType.Buffer]    = ll.impl.castBufferToObject,
        [ll.ObjectType.Image]     = ll.impl.castImageToObject,
        [ll.ObjectType.ImageView] = ll.impl.castImageViewToObject
    }

    self:__bind(name, castTable[obj.type](obj))
end


function ll.ContainerNode:getNode(name)
    return ll.castNode(self:__getNode(name))
end


function ll.ContainerNode:bindNode(name, node)

    castTable = {
        [ll.NodeType.Compute]   = ll.impl.castComputeNodeToNode,
        [ll.NodeType.Container] = ll.impl.castContainerNodeToNode
    }

    self:__bindNode(name, castTable[node.type](node))
end
