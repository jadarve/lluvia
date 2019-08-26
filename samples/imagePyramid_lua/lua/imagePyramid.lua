local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc:builderName = 'imagePyramid'

    desc:addPort(ll.PortDescriptor.new(0, 'in_RGBA', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_RGBA', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:addParameter(ll.ParameterDescriptor.new('levels', ll.ParameterType.Int, 0))

    return desc
end


function builder.onNodeInit(node)

    levels = node.descriptor:getParameter('levels')

    -- in_RGBA should have been bound before calling init()
    in_RGBA = node:getPort('in_RGBA')

    for i = 0, levels -1 do
        
        downX = ll.createComputeNode('imageDownsampleX')
        downY = ll.createComputeNode('imageDownsampleX')

        downX:bind('in_RGBA', in_RGBA)
        downX:init()

        downY:bind('in_RGBA', downX:getPort('out_RGBA'))
        downY:init()

        -- bind the output
        if i == levels -1 then
            node:bind('out_RGBA', downY:getPort('out_RGBA'))
        end

        node:addNode(string.format('downX_%d', i), downX)
        node:addNode(string.format('downY_%d', i), downY)

        node:addObject(string.format('out_RGBA_downX_%d', i), downX:getPort('out_RGBA'))
        node:addObject(string.format('out_RGBA_downY_%d', i), downY:getPort('out_RGBA'))
    end

end


function builder.onNodeRecord(node, cmdBuffer)

    levels = node.descriptor:getParameter('levels')

    for i = 0, levels -1 do
        
        downX = node:getNode(string.format('downX_%d', i))
        downY = node:getNode(string.format('downY_%d', i))

        cmdBuffer:run(downX)
        cmdBuffer:memoryBarrier()
        cmdBuffer:run(downY)
        cmdBuffer:memoryBarrier()
    end
end


ll.registerNodeBuilder(builder)
