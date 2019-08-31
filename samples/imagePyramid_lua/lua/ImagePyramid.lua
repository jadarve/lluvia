local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    ll.logd('ImagePyramid', 'newDescriptor')
    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = 'ImagePyramid'

    desc:addPort(ll.PortDescriptor.new(0, 'in_RGBA', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_RGBA', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:setParameter('levels', 2)

    return desc
end


function builder.onNodeInit(node)

    levels = node.descriptor:getParameter('levels')
    ll.logd('ImagePyramid', 'onNodeInit: levels:', levels)

    -- in_RGBA should have been bound before calling init()
    in_RGBA = node:getPort('in_RGBA')
    node:bind(string.format('out_RGBA_downY_0', i), in_RGBA)

    for i = 1, levels -1 do
        ll.logd('ImagePyramid', 'onNodeInit: level:', i)
        
        downX = ll.createComputeNode('ImageDownsampleX')
        downY = ll.createComputeNode('ImageDownsampleY')

        downX:bind('in_RGBA', in_RGBA)
        downX:init()

        downY:bind('in_RGBA', downX:getPort('out_RGBA'))
        downY:init()

        in_RGBA = downY:getPort('out_RGBA')

        -- bind the output
        if i == levels -1 then
            node:bind('out_RGBA', downY:getPort('out_RGBA'))
        end

        node:bindNode(string.format('downX_%d', i), downX)
        node:bindNode(string.format('downY_%d', i), downY)

        node:bind(string.format('out_RGBA_downX_%d', i), downX:getPort('out_RGBA'))
        node:bind(string.format('out_RGBA_downY_%d', i), downY:getPort('out_RGBA'))
    end

end


function builder.onNodeRecord(node, cmdBuffer)

    levels = node.descriptor:getParameter('levels')

    for i = 1, levels -1 do
        ll.logd('ImagePyramid', 'onNodeRecord: level:', i)

        downX = node:getNode(string.format('downX_%d', i))
        downY = node:getNode(string.format('downY_%d', i))

        downX:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
        downY:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
    end
end


ll.registerNodeBuilder('ImagePyramid', builder)
