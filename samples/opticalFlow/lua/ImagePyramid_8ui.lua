local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    ll.logd('ImagePyramid_8ui', 'newDescriptor')
    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = 'ImagePyramid_8ui'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:setParameter('levels', 1)

    return desc
end


function builder.onNodeInit(node)

    levels = node.descriptor:getParameter('levels')
    ll.logd('ImagePyramid_8ui', 'onNodeInit: levels:', levels)

    -- in_gray should have been bound before calling init()
    in_gray = node:getPort('in_gray')
    node:bind(string.format('out_gray_0', i), in_gray)

    -- Pass through the input to the output
    if levels == 1 then
        node:bind('out_gray', in_gray)
    end

    for i = 1, levels -1 do
        ll.logd('ImagePyramid_8ui', 'onNodeInit: level:', i)
        
        downX = ll.createComputeNode('ImageDownsampleX_8ui')
        downY = ll.createComputeNode('ImageDownsampleY_8ui')

        downX:bind('in_gray', in_gray)
        downX:init()

        downY:bind('in_gray', downX:getPort('out_gray'))
        downY:init()

        in_gray = downY:getPort('out_gray')

        -- bind the output
        if i == levels -1 then
            node:bind('out_gray', downY:getPort('out_gray'))
        end

        ll.logd('ImagePyramid_8ui', 'onNodeInit: level:', i, 'binding node')
        node:bindNode(string.format('DownX_%d', i), downX)
        node:bindNode(string.format('DownY_%d', i), downY)

        -- outputs of each level
        node:bind(string.format('out_gray_%d', i), downY:getPort('out_gray'))
    end

end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd('ImagePyramid_8ui', 'onNodeRecord')

    levels = node.descriptor:getParameter('levels')

    for i = 1, levels -1 do
        ll.logd('ImagePyramid_8ui', 'onNodeRecord: level:', i)

        downX = node:getNode(string.format('DownX_%d', i))
        downY = node:getNode(string.format('DownY_%d', i))

        downX:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
        downY:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
    end

    ll.logd('ImagePyramid_8ui', 'onNodeRecord: finish')
end


ll.registerNodeBuilder('ImagePyramid_8ui', builder)
