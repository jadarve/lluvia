local builder = ll.class(ll.ContainerNodeBuilder)


function builder.newDescriptor()

    ll.logd('ImagePyramid_gray', 'newDescriptor')
    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = 'ImagePyramid_gray'

    desc:addPort(ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:addParameter('levels', 1)

    return desc
end


function builder.onNodeInit(node)

    levels = node.descriptor:getParameter('levels')
    ll.logd('ImagePyramid_gray', 'onNodeInit: levels:', levels)

    -- in_gray should have been bound before calling init()
    in_gray = node:getPort('in_gray')
    node:bind(string.format('out_gray_0', i), in_gray)

    for i = 1, levels -1 do
        ll.logd('ImagePyramid_gray', 'onNodeInit: level:', i)
        
        downX = ll.createComputeNode('ImageDownsampleX_gray')
        downY = ll.createComputeNode('ImageDownsampleY_gray')

        downX:bind('in_gray', in_gray)
        downX:init()

        downY:bind('in_gray', downX:getPort('out_gray'))
        downY:init()

        in_gray = downY:getPort('out_gray')

        -- bind the output
        if i == levels -1 then
            node:bind('out_gray', downY:getPort('out_gray'))
        end

        node:bindNode(string.format('downX_%d', i), downX)
        node:bindNode(string.format('downY_%d', i), downY)

        -- outputs of each level
        node:bind(string.format('out_gray_%d', i), downY:getPort('out_gray'))
    end

end


function builder.onNodeRecord(node, cmdBuffer)

    levels = node.descriptor:getParameter('levels')

    for i = 1, levels -1 do
        ll.logd('ImagePyramid_gray', 'onNodeRecord: level:', i)

        downX = node:getNode(string.format('downX_%d', i))
        downY = node:getNode(string.format('downY_%d', i))

        downX:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
        downY:record(cmdBuffer)
        cmdBuffer:memoryBarrier()
    end
end


ll.registerNodeBuilder('ImagePyramid_gray', builder)
