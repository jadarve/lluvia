
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


function ll.isValidImage(img, expectedChannelCount, expectedChannelType)

    local desc = img.imageDescriptor
    
    if desc.channelType ~= expectedChannelType then
        return "expecting channel type " .. expectedChannelType .. ' got: ' .. desc.channelType
    end

    if desc.channelCount ~= expectedChannelCount then
        return "expecting channel count " .. expectedChannelCount .. ' got: ' .. desc.channelCount
    end

    return nil
end
-----------------------------------------------------------
--                     Session
-----------------------------------------------------------
function ll.getHostMemory()
    if not ll.activeSession then
        error('ll.activeSession nil')
    end

    return ll.activeSession:getHostMemory()
end

function ll.getProgram(name)
    
    if not ll.activeSession then
        error('ll.activeSession nil')
    end

    return ll.activeSession:getProgram(name)
end

function ll.createComputeNode(name)
    
    if not ll.activeSession then
        error('ll.activeSession nil')
    end

    return ll.activeSession:createComputeNode(name)
end

function ll.createContainerNode(name)
    
    if not ll.activeSession then
        error('ll.activeSession nil')
    end

    return ll.activeSession:createContainerNode(name)
end

function ll.getGoodComputeLocalShape(dimensions)

    if not ll.activeSession then
        error('ll.activeSession nil')
    end

    return ll.activeSession:getGoodComputeLocalShape(dimensions)
end


-----------------------------------------------------------
--                 ComputeNodeBuilder
-----------------------------------------------------------
ll.ComputeNodeBuilder = ll.class()

ll.ComputeNodeBuilder.type = ll.NodeType.Compute
ll.ComputeNodeBuilder.doc  = ""

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

ll.ContainerNodeBuilder.type = ll.NodeType.Container
ll.ContainerNodeBuilder.doc  = ""

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
--                     Parameter
-----------------------------------------------------------

function ll.Parameter:get()

    -- castTable = {
    --     [ll.ParameterType.Int]   = self:__getInt(),
    --     [ll.ParameterType.Float] = self:__getFloat()
    -- }

    -- return castTable[self.type]()

    if self.type == ll.ParameterType.Int then
        return self:__getInt()
    end

    if self.type == ll.ParameterType.Float then
        return self:__getFloat()
    end

    if self.type == ll.ParameterType.Bool then
        return self:__getBool()
    end
end


function ll.Parameter:set(value)

    if type(value) == 'number' then
        self:__setFloat(value)
    end

    if type(value) == 'boolean' then
        self:__setBool(value)
    end

    -- castTable = {
    --     ['number'] = self:__setFloat
    -- }

    -- castTable[type(value)](value)
end


-----------------------------------------------------------
--                ComputeNodeDescriptor
-----------------------------------------------------------

--- Initialize the descriptor.
--
-- The initialization includes:
-- - Setting the descriptor buildderName to name
-- - Looking for the program with the same name in the registry.
-- - Setting program functionName to main.
-- - Setting the gridShape to (1, 1, 1)
-- - Setting the localShape to 
--
-- @param name The name of the descriptor. It is also used
--             for looking for the program in the registry.
function ll.ComputeNodeDescriptor:init(name, dimensions)

    self.builderName  = name
    self.program      = ll.getProgram(name)
    self.functionName = 'main'

    self.localShape   = ll.getGoodComputeLocalShape(dimensions)
    self.gridShape    = ll.vec3ui.new(1, 1, 1)
end

function ll.ComputeNodeDescriptor:setParameter(name, value)

    -- this workaround is needed in order to call
    -- the correct setter method given Lua type for value
    local param = ll.Parameter.new()
    param:set(value)

    self:__setParameter(name, param)
end


function ll.ComputeNodeDescriptor:getParameter(name)

    local param = self:__getParameter(name)
    return param:get()
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


function ll.ComputeNode:setParameter(name, value)

    -- this workaround is needed in order to call
    -- the correct setter method given Lua type for value
    local param = ll.Parameter.new()
    param:set(value)

    self:__setParameter(name, param)
end


function ll.ComputeNode:getParameter(name)

    local param = self:__getParameter(name)
    return param:get()
end


-----------------------------------------------------------
--                ContainerNodeDescriptor
-----------------------------------------------------------
function ll.ContainerNodeDescriptor:setParameter(name, value)

    -- this workaround is needed in order to call
    -- the correct setter method given Lua type for value
    local param = ll.Parameter.new()
    param:set(value)

    self:__setParameter(name, param)
end


function ll.ContainerNodeDescriptor:getParameter(name)

    local param = self:__getParameter(name)
    return param:get()
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
    
    local node = self:__getNode(name)
    if not node then
        error(string.format('node with name %s not found', name))
    end
    
    return ll.castNode(node)
end


function ll.ContainerNode:bindNode(name, node)

    castTable = {
        [ll.NodeType.Compute]   = ll.impl.castComputeNodeToNode,
        [ll.NodeType.Container] = ll.impl.castContainerNodeToNode
    }

    self:__bindNode(name, castTable[node.type](node))
end

function ll.ContainerNode:setParameter(name, value)

    -- this workaround is needed in order to call
    -- the correct setter method given Lua type for value
    local param = ll.Parameter.new()
    param:set(value)

    self:__setParameter(name, param)
end


function ll.ContainerNode:getParameter(name)

    local param = self:__getParameter(name)
    return param:get()
end
