import pytest

import numpy as np

import lluvia as ll
import lluvia.util as ll_util
import lluvia_test as ll_test


def alignStd140(arr):
    """
    Align a 3x3 matrix according to the GLSL STD140 standard
    """
    
    out = np.zeros((4,3), dtype=np.float32, order='F')
    out[:3, :3] = arr
    return out

def createCameraUniformBuffer(width, height):
    """
    Creates the camera uniform buffer according to the width and height of the camera.
    """

    K = np.array([[width, 0, 0.5*(width -1)],
                [0, height, 0.5*(height -1)],
                [0, 0, 1]], dtype=np.float32, order='F')
    Kinv = np.linalg.inv(K)

    Kg = alignStd140(K)
    Kinvg = alignStd140(Kinv)
    radialDistortion = np.array([0, 0, 0, 0,], dtype=np.float32)
    tangentialDistortion = np.array([0, 0, 0, 0], dtype=np.float32)

    # create bytes buffer from matrices
    buf = Kg.tobytes(order='F') + Kinvg.tobytes(order='F') + radialDistortion.tobytes() + tangentialDistortion.tobytes()
    npBuf = np.frombuffer(buf, dtype=np.uint8)

    return npBuf

def test_goodUse():

    nodeName = 'lluvia/camera/CameraRectification_rgba'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/camera/CameraRectification_rgba.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/camera/CameraRectification_rgba.comp.spv',
                     programName='lluvia/camera/CameraRectification_rgba.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)
    host_memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal, ll.MemoryPropertyFlagBits.HostVisible, ll.MemoryPropertyFlagBits.HostCoherent])

    imgRGBA = np.zeros((864, 1536, 4), dtype=np.uint8)
    in_rgba = memory.createImageViewFromHost(imgRGBA,
        filterMode=ll.ImageFilterMode.Nearest,
        addressMode=ll.ImageAddressMode.ClampToEdge,
        normalizedCoordinates=False,
        sampled=True)

    # TODO: replace this for a Camera class once I have one
    in_camera_host = createCameraUniformBuffer(float(in_rgba.width), float(in_rgba.height))
    in_camera = host_memory.createBufferFromHost(in_camera_host, usageFlags=[ll.BufferUsageFlagBits.TransferSrc, ll.BufferUsageFlagBits.TransferDst, ll.BufferUsageFlagBits.UniformBuffer])

    node.bind('in_rgba', in_rgba)
    node.bind('in_camera', in_camera)
    node.init()

    out_rgba = node.getPort('out_rgba')
    assert(out_rgba is not None)
    assert(out_rgba.width == in_rgba.width)
    assert(out_rgba.height == in_rgba.height)
    assert(out_rgba.depth == in_rgba.depth)
    assert(out_rgba.channelType == ll.ChannelType.Uint8)
    assert(out_rgba.channels == 4)

    session.run(node)

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
