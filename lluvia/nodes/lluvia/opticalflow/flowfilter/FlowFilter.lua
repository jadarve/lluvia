local builder = ll.class(ll.ContainerNodeBuilder)

builder.name = 'lluvia/opticalflow/flowfilter/FlowFilter'
builder.doc = [[
An optical flow filter with pyramidal implementation.

Parameters
----------
max_flow : float. Defaults to 1.0
    The max magnitude allowed for the optical flow output.

levels : int. Defaults to 1.
    The number of levels to create. If the value is 1, the in_gray input is bound
    as out_gray in the output, without any memory copy.

gamma : float. Defaults to 0.01
    The filter gain for the update step at the top level of the pyramid

gamma_low : float. Defaults to 0.01
    The filter gains for the update step at the intermediate pyramid levels.

smooth_iterations : int. Defaults to 1.
    The number of smooth iterations to apply to the estimated flow at each pyramid level.

float_precision : int. Defaults to ll.FloatPrecision.FP32.
    Floating point precision used accross the algorithm. The outputs out_gray and out_flow
    will be of this floating point precision.

Inputs
------
in_gray : ImageView
    r8ui image. The input gray-scale image.

Outputs
-------
out_gray : ImageView
    {r16f, r32f} image. The gray-scale image after one iteration of the algorithm. The floating
    point precision of this output is affected by the float_precision parameter.

out_flow : ImageView
    {rg16f, rg32f} image. The estimated optical flow. The floating
    point precision of this output is affected by the float_precision parameter.

]]

function builder.newDescriptor()

    local desc = ll.ContainerNodeDescriptor.new()

    desc.builderName = builder.name

    local in_gray = ll.PortDescriptor.new(0, 'in_gray', ll.PortDirection.In, ll.PortType.ImageView)
    in_gray:checkImageChannelCountIs(ll.ChannelCount.C1)
    in_gray:checkImageChannelTypeIs(ll.ChannelType.Uint8)

    desc:addPort(in_gray)
    desc:addPort(ll.PortDescriptor.new(1, 'out_gray', ll.PortDirection.Out, ll.PortType.ImageView))
    desc:addPort(ll.PortDescriptor.new(2, 'out_flow', ll.PortDirection.Out, ll.PortType.ImageView))

    -- parameter with default value
    desc:setParameter('max_flow', 1)
    desc:setParameter('levels', 1)
    desc:setParameter('gamma', 0.01)
    desc:setParameter('gamma_low', 0.01)
    desc:setParameter('smooth_iterations', 1)
    desc:setParameter('float_precision', ll.FloatPrecision.FP32)

    return desc
end


function builder.onNodeInit(node)

    ll.logd(node.descriptor.builderName, 'onNodeInit')

    local gamma = node:getParameter('gamma')
    local gamma_low = node:getParameter('gamma_low')
    local max_flow = node:getParameter('max_flow')
    local smooth_iterations = node:getParameter('smooth_iterations')
    local levels = node:getParameter('levels')
    local float_precision = node:getParameter('float_precision')

    ll.logd(node.descriptor.builderName, 'onNodeInit: max_flow', max_flow)

    local in_gray = node:getPort('in_gray')


    local imagePyramid = ll.createContainerNode('lluvia/imgproc/ImagePyramid_r8ui')
    imagePyramid:setParameter('levels', levels)
    imagePyramid:bind('in_gray', in_gray)
    imagePyramid:init()
    
    local filterTop = ll.createContainerNode('lluvia/opticalflow/flowfilter/FlowFilterSimple')
    filterTop:setParameter('gamma', gamma)
    filterTop:setParameter('max_flow', math.ceil(max_flow / (2^(levels - 1))))
    filterTop:setParameter('smooth_iterations', smooth_iterations)
    filterTop:setParameter('float_precision', float_precision)

    -- use the top level output of the pyramid as input to filterTop
    filterTop:bind('in_gray', imagePyramid:getPort('out_gray'))
    filterTop:init()

    node:bind(string.format('out_flow_%d', levels - 1), filterTop:getPort('out_flow'))
    node:bind(string.format('out_gray_%d', levels - 1), filterTop:getPort('out_gray'))

    local out_flow = filterTop:getPort('out_flow')
    local out_gray = filterTop:getPort('out_gray')

    local h = levels - 2
    while h >= 0 do

        local filterLow = ll.createContainerNode('lluvia/opticalflow/flowfilter/FlowFilterDelta')
        filterLow:setParameter('gamma', gamma_low)
        filterLow:setParameter('max_flow', math.ceil(max_flow / (2^h)))
        filterLow:setParameter('smooth_iterations', smooth_iterations)
        filterLow:setParameter('float_precision', float_precision)

        local in_gray = imagePyramid:getPort(string.format('out_gray_%d', h))
        filterLow:bind('in_gray', in_gray)
        filterLow:bind('in_flow', out_flow)

        filterLow:init()
        node:bindNode(string.format('FilterLow_%d', h), filterLow)

        node:bind(string.format('out_gray_%d', h), filterLow:getPort('out_gray'))
        node:bind(string.format('out_flow_%d', h), filterLow:getPort('out_flow'))
        node:bind(string.format('out_delta_flow_%d', h), filterLow:getPort('out_delta_flow'))

        out_flow = filterLow:getPort('out_flow')
        out_gray = filterLow:getPort('out_gray')

        h = h - 1
    end

    node:bindNode('ImagePyramid', imagePyramid)
    node:bindNode('FilterTop', filterTop)

    node:bind('out_flow', out_flow)
    node:bind('out_gray', out_gray)

    ll.logd(node.descriptor.builderName, 'onNodeInit: finish')
end


function builder.onNodeRecord(node, cmdBuffer)

    ll.logd(node.descriptor.builderName, 'onNodeRecord')

    local levels = node:getParameter('levels')

    local imagePyramid = node:getNode('ImagePyramid')
    local filterTop = node:getNode('FilterTop')

    imagePyramid:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    filterTop:record(cmdBuffer)
    cmdBuffer:memoryBarrier()

    local h = levels - 2
    while h >= 0 do

        local filterLow = node:getNode(string.format('FilterLow_%d', h))
        filterLow:record(cmdBuffer)
        cmdBuffer:memoryBarrier()

        h = h - 1
    end

    ll.logd(node.descriptor.builderName, 'onNodeRecord: finish')
end


ll.registerNodeBuilder(builder)
