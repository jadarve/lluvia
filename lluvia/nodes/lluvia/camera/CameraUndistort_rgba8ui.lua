local builder = ll.class(ll.ComputeNodeBuilder)

builder.name = 'lluvia/camera/CameraUndistort_rgba8ui'
builder.doc = [[
Rectifies an RGBA input image applying camera distortion model.

### Standard distortion model

For the standard camera model, the undistortion process is as follows.
A 3D point $\mathbf{x} \in \mathbb{R}^3$ is expressed relative to the camera body fixed frame.
It projects onto the camera image plane as pixel $\mathbf{p} := (u, v)^\top \in \mathbb{R}^2$ as

$$
\begin{equation}
\begin{pmatrix}
    \mathbf{p} \\\
    1
\end{pmatrix} := \begin{pmatrix}
    u \\\
    v \\\
    1
\end{pmatrix} = \frac{\mathbf{K} \mathbf{x}}{ \left< e_3, \mathbf{x} \right>}
\end{equation}
$$

where $\mathbf{K} \in \mathbb{R}^{3\times3}$ is the camera intrinsics matrix, $e_3 := (0, 0, 1)^\top$, and $\left< e_3, \mathbf{x} \right>$ is the dot product between the two vectors. The units of $\mathbf{p}$ are actual pixel coordinates in the ranges $u \in [0, W)$ and $v \in [0, H)$, with $W$ and $H$ denoting the image width and height respectively.

Given a pixel point, the corresponding 3D coordinate $\bar{\mathbf{x}}$ in the image plane is defined as:

$$
\begin{equation}
\bar{\mathbf{x}} := \begin{pmatrix}
\bar{x} \\\
\bar{y} \\\
\bar{z} \\\
\end{pmatrix} = \mathbf{K}^{-1} \begin{pmatrix}
    \mathbf{p} \\\
    1
\end{pmatrix}
\end{equation}
$$

The standard distortion model is formed by two components:

* A **radial** component parameterized by three coefficients: $k_1$, $k_2$, and $k_3$.
* A **tangential** component with two parameters: $p_1$ and $p_2$.

The radial distortion component for a given pixel $\mathbf{p}$ is computed as

$$
\begin{equation}
\bar{\mathbf{x}}_r :=
R \begin{pmatrix}
  \bar{x} \\\
  \bar{y} \\\
  0
\end{pmatrix}
\end{equation}
$$

where $R \in \mathbb{R}$ is

$$
\begin{equation}
R = k_1 r^2 + k_2 r^4 + k_3 r^6 
\end{equation}
$$

with

$$
\begin{equation}
r^2 = \bar{x}^2 + \bar{y}^2
\end{equation}
$$

and $\bar{x}, \bar{y}$ are the $x$ and $y$ coordinates of the projection of pixel $\mathbf{p}$ using equation (2).

The tangential distortion is computed as:

$$
\begin{equation}
\bar{\mathbf{x}}_p :=
\begin{pmatrix}
  2 p_1 \bar{x}\bar{y} + p_2(r^2 + 2\bar{x}^2) \\\
  p_1(r^2 + 2 \bar{y}^2) + 2 p_2 \bar{x}\bar{y} \\\
  0
\end{pmatrix}
\end{equation}
$$

Finally, the undistorted image plane coordinates $\bar{\mathbf{x}}_u$ is computed as:

$$
\begin{equation}
\bar{\mathbf{x}}_u = \bar{\mathbf{x}} + \bar{\mathbf{x}}_r + \bar{\mathbf{x}}_p
\end{equation}
$$

Given $\bar{\mathbf{x}}_u$, the corresponding undistorted pixel coordinate is:

$$
\begin{equation}
\begin{pmatrix}
    \mathbf{p}_u \\\
    1
\end{pmatrix} := \begin{pmatrix}
    u_u \\\
    v_u \\\
    1
\end{pmatrix} = \frac{\mathbf{K} \bar{\mathbf{x}}_u}{ \left< e_3, \bar{\mathbf{x}}_u \right>}
\end{equation}
$$


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
    Uniform buffer containing the camera data. The buffer is interpreted as GLSL `ll_camera` struct.
    The alignment of the struct follows the GLSL std140 rules of alignment.

```glsl
struct ll_camera {
    mat3 K;
    mat3 Kinv;
    vec4 radialDistortion;
    vec4 tangentialDistortion;
};
```

Outputs
-------
out_rgba : ImageView
    rgba8ui image. The rectified image.

Examples
--------

```python
import lluvia as ll
import lluvia.util as ll_util
import numpy as np
import matplotlib.pyplot as plt

session = ll.createSession()

# memory to store the input and output images
memory = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

# memory to store the uniform buffer with the camera parameters
host_memory = session.createMemory([ll.MemoryPropertyFlagBits.DeviceLocal,
                                    ll.MemoryPropertyFlagBits.HostVisible,
                                    ll.MemoryPropertyFlagBits.HostCoherent])

# read a sample image
sampleImage = ll_util.readSampleImage('koala')

# draw a grid on top of the sample image
Yrange = np.arange(0, sampleImage.shape[0], 128)
Ylines = np.concatenate([n + Yrange for n in range(4)])

Xrange = np.arange(0, sampleImage.shape[1], 128)
Xlines = np.concatenate([n + Xrange for n in range(4)])

sampleImage[Ylines, ...] = 0
sampleImage[:, Xlines, ...] = 0

# the input image view must be sampled. This example uses nearest neighbor interpolation
in_rgba = memory.createImageViewFromHost(sampleImage,
                                         filterMode=ll.ImageFilterMode.Nearest,
                                         addressMode=ll.ImageAddressMode.Repeat,
                                         normalizedCoordinates=False,
                                         sampled=True)

###################################################
# Camera parameters
W = float(in_rgba.width)
H = float(in_rgba.height)

# Dummy camera matrix
K = np.array([[W, 0, 0.5*(W -1)],
              [0, H, 0.5*(H -1)],
              [0, 0, 1] ], dtype=np.float32, order='F')
Kinv = np.linalg.inv(K)
radialDistortion = np.array([0.5, 0, 0, 0,], dtype=np.float32)
tangentialDistortion = np.array([0.1, 0, 0, 0], dtype=np.float32)

# align the matrices according to the STD140 rules (column major, 4-component vectors)
K_aligned = np.zeros((4,3), dtype=np.float32, order='F'); K_aligned[:3, :3] = K
Kinv_aligned = np.zeros((4,3), dtype=np.float32, order='F'); Kinv_aligned[:3, :3] = Kinv

# create bytes buffer from matrices
buf = K_aligned.tobytes(order='F') + Kinv_aligned.tobytes(order='F') + radialDistortion.tobytes() + tangentialDistortion.tobytes()
npBuf = np.frombuffer(buf, dtype=np.uint8)

# in_camera uniform buffer
in_camera = host_memory.createBufferFromHost(npBuf, usageFlags=[ll.BufferUsageFlagBits.TransferSrc,
                                                                ll.BufferUsageFlagBits.TransferDst,
                                                                ll.BufferUsageFlagBits.UniformBuffer])

###################################################
# Compute node
CameraUndistort = session.createComputeNode('lluvia/camera/CameraUndistort_rgba8ui')
CameraUndistort.setParameter('camera_model', ll.Parameter(1)) # standard model
CameraUndistort.bind('in_rgba', in_rgba)
CameraUndistort.bind('in_camera', in_camera)
CameraUndistort.init()

CameraUndistort.run()

out_rgba = CameraUndistort.getPort('out_rgba')

fig = plt.figure(figsize=(30, 15)); fig.set_tight_layout(True)
plt.subplot2grid((1,2), (0,0)); plt.imshow(in_rgba.toHost()[..., :3])
plt.subplot2grid((1,2), (0,1)); plt.imshow(out_rgba.toHost()[..., :3])
plt.show()
```

References
----------

* Zhang, Z., 2000. A flexible new technique for camera calibration. IEEE Transactions on pattern analysis and machine intelligence, 22(11), pp.1330-1334.
* Wei, G.Q. and De Ma, S., 1994. Implicit and explicit camera calibration: Theory and experiments. IEEE Transactions on Pattern Analysis and Machine Intelligence, 16(5), pp.469-480.
* Szeliski, R., 2010. Computer vision: algorithms and applications. Springer Science & Business Media.
* Hartley, R. and Zisserman, A., 2003. Multiple view geometry in computer vision. Cambridge university press.
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
