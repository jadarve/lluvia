import pytest

import numpy as np

import lluvia as ll
import lluvia.util as ll_util
import lluvia_test as ll_test


@pytest.mark.parametrize(
    "inputChannelType,outputChannelType,max_value,check_pixels", [
        pytest.param(ll.ChannelType.Uint8, ll.ChannelType.Float16, 0.0, True, id="uint8_to_float16"),
        pytest.param(ll.ChannelType.Uint8, ll.ChannelType.Float32, 0.0, True, id="uint8_to_float32"),

        pytest.param(ll.ChannelType.Uint16, ll.ChannelType.Float16, 0.0, True, id="uint16_to_float16"),
        pytest.param(ll.ChannelType.Uint16, ll.ChannelType.Float32, 0.0, True, id="uint16_to_float32"),

        pytest.param(ll.ChannelType.Uint32, ll.ChannelType.Float16, 0.0, True, id="uint32_to_float16"),
        pytest.param(ll.ChannelType.Uint32, ll.ChannelType.Float32, 0.0, True, id="uint32_to_float32"),

        pytest.param(ll.ChannelType.Uint8, ll.ChannelType.Float16, 255, False, id="uint8_to_float16_normalized"), # Do not check pixels for float16
        pytest.param(ll.ChannelType.Uint8, ll.ChannelType.Float32, 255, True, id="uint8_to_float32_normalized"),

        pytest.param(ll.ChannelType.Uint16, ll.ChannelType.Float16, 255, False, id="uint16_to_float16_normalized"), # Do not check pixels for float16
        pytest.param(ll.ChannelType.Uint16, ll.ChannelType.Float32, 255, True, id="uint16_to_float32_normalized"),

        pytest.param(ll.ChannelType.Uint32, ll.ChannelType.Float16, 255, False, id="uint32_to_float16_normalized"), # Do not check pixels for float16
        pytest.param(ll.ChannelType.Uint32, ll.ChannelType.Float32, 255, True, id="uint32_to_float32_normalized"),
    ],
)
def test_goodUse(inputChannelType, outputChannelType, max_value, check_pixels):

    npInputDtype = {
        ll.ChannelType.Uint8 : np.uint8,
        ll.ChannelType.Uint16 : np.uint16,
        ll.ChannelType.Uint32 : np.uint32,
    }[inputChannelType]

    npOutpuDtype = {
        ll.ChannelType.Float16 : np.float16,
        ll.ChannelType.Float32 : np.float32,
    }[outputChannelType]

    nodeName = 'lluvia/math/normalize/ImageNormalize_uint_C1'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/math/normalize/ImageNormalize_uint_C1.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/math/normalize/ImageNormalize_uint_C1.comp.spv',
                     programName='lluvia/math/normalize/ImageNormalize_uint_C1.comp'
                     )

    node = session.createComputeNode(nodeName)

    memory = session.createMemory(
        flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    imgGray = ll_util.readGray('lluvia/resources/mouse.jpg').astype(npInputDtype)
    in_image_uint = memory.createImageViewFromHost(imgGray)

    out_image_float = memory.createImage(in_image_uint.shape, outputChannelType).createImageView()

    node.bind('in_image_uint', in_image_uint)
    node.bind('out_image_float', out_image_float)
    node.setParameter('max_value', ll.Parameter(max_value))
    node.init()
    session.run(node)

    assert(out_image_float is not None)
    assert(out_image_float.width == in_image_uint.width)
    assert(out_image_float.height == in_image_uint.height)
    assert(out_image_float.depth == in_image_uint.depth)
    assert(out_image_float.channelType == outputChannelType)
    assert(out_image_float.channels == 1)

    # Convert the input to float32, perform the normalization.
    # Then convert to whatever floating point precision of the output (npOutputDtype) and
    # then back to float32.
    # This process is similar to what it is performed in the compute shader.
    host_in_image_float = in_image_uint.toHost().astype(np.float32)
    if max_value > 0:
        host_in_image_float /= max_value
    
    host_in_image_float32 = host_in_image_float.astype(npOutpuDtype).astype(np.float32)

    host_out_image_float32 = out_image_float.toHost().astype(np.float32)

    # pixel-wise check. This is needed as the CPU normalization for float16 have a higher difference than the default
    # tolerances of the np.allclose function.
    if check_pixels:
        assert(np.allclose(host_in_image_float32, host_out_image_float32))

    assert(not session.hasReceivedVulkanWarningMessages())



if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
