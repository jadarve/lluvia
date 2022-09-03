local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/camera/CameraUndistort_rgba'
builder.doc = [[
Rectifies an RGBA input image applying camera distortion model.

The node 

Parameters
----------
camera_model : int. Defaults to 0.
    The camera model used for rectifying the image. Possible values are:

    * 0: standard model

Inputs
------
in_rgba : ImageView.
    rgba8ui image.

in_camera : UniformBuffer.
    Uniform buffer containing the camera data. The buffer is interpreted as GLSL `ll_camera` struct:

    ```glsl
    struct ll_camera {
        mat3 K;
        mat3 Kinv;
        vec4 radialDistortion;
        vec4 tangentialDistortion;
    };
    ```

    The alignment of the struct follows the GLSL std140 rules of alignment.

Outputs
-------
out_rgba : ImageView
    rgba8ui image. The rectified image.

Examples
--------

```python

```

]]

function builder.newDescriptor()

    local desc = ll.ComputeNodeDescriptor.new()

    desc:init(builder.name, ll.ComputeDimension.D2)

    local in_rgba = ll.PortDescriptor.new(0, 'in_rgba', ll.PortDirection.In, ll.PortType.SampledImageView)
    in_rgba:checkImageChannelCountIs(ll.ChannelCount.C4)
    in_rgba:checkImageChannelTypeIs(ll.ChannelType.Uint8)

    local in_camera = ll.PortDescriptor.new(1, 'in_camera', ll.PortDirection.In, ll.PortType.UniformBuffer)

    local out_rgba = ll.PortDescriptor.new(2, 'out_rgba', ll.PortDirection.Out, ll.PortType.ImageView)
    out_rgba:checkImageChannelCountIs(ll.ChannelCount.C4)
    out_rgba:checkImageChannelTypeIs(ll.ChannelType.Uint8)
    
    desc:addPort(in_rgba)
    desc:addPort(in_camera)
    desc:addPort(out_rgba)

    desc:setParameter('camera_model', 0)

    return desc
end

function builder.onNodeInit(node)

    local in_rgba = node:getPort('in_rgba')

    local camera_model = node:getParameter('camera_model')

    -- push constants
    local pushConstants = ll.PushConstants.new()
    pushConstants:pushInt32(math.tointeger(camera_model))

    node.pushConstants = pushConstants
    
    -------------------------------------------------------
    -- allocate out_rgba
    -------------------------------------------------------
    local width = in_rgba.width
    local height = in_rgba.height
    local depth = in_rgba.depth

    local imgDesc = ll.ImageDescriptor.new()
    imgDesc.width = width
    imgDesc.height = height
    imgDesc.depth = depth
    imgDesc.channelCount = ll.ChannelCount.C4
    imgDesc.channelType = ll.ChannelType.Uint8

    local imgViewDesc = ll.ImageViewDescriptor.new()
    imgViewDesc.filterMode = ll.ImageFilterMode.Nearest
    imgViewDesc.normalizedCoordinates = false
    imgViewDesc.isSampled = false
    imgViewDesc:setAddressMode(ll.ImageAddressMode.Repeat)

    local memory = in_rgba.memory
    local out_rgba = memory:createImageView(imgDesc, imgViewDesc)

    -- need to change image layout before binding
    out_rgba:changeImageLayout(ll.ImageLayout.General)

    node:bind('out_rgba', out_rgba)
    node:configureGridShape(ll.vec3ui.new(out_rgba.width, out_rgba.height, 1))
end

-- register builder in the system
ll.registerNodeBuilder(builder)
