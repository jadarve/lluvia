import pytest
import numpy as np

import lluvia as ll
import lluvia_test as ll_test

@pytest.mark.parametrize(
    "dtype", [
        pytest.param(np.uint8, id="uint8"),
        pytest.param(np.uint16, id="uint16"),
        pytest.param(np.uint32, id="uint32"),
        pytest.param(np.int8, id="int8"),
        pytest.param(np.int16, id="int16"),
        pytest.param(np.int32, id="int32"),
        pytest.param(np.float16, id="float16"),
        pytest.param(np.float32, id="float32"),
    ],
)
def test_goodUse(dtype):

    nodeName = 'lluvia/viz/colormap/ColorMap'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/viz/colormap/ColorMap_uint.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/viz/colormap/ColorMap_uint.comp.spv',
                     programName='lluvia/viz/colormap/ColorMap_uint.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/viz/colormap/ColorMap_int.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/viz/colormap/ColorMap_int.comp.spv',
                     programName='lluvia/viz/colormap/ColorMap_int.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/viz/colormap/ColorMap_float.lua',
                     programPath='lluvia/lluvia/nodes/lluvia/viz/colormap/ColorMap_float.comp.spv',
                     programName='lluvia/viz/colormap/ColorMap_float.comp'
                     )
    ll_test.loadNode(session,
                     builderPath='lluvia/lluvia/nodes/lluvia/viz/colormap/ColorMap.lua'
                     )

    memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    in_image = memory.createImageViewFromHost(np.zeros((480, 640, 1), dtype=dtype))

    supported_colormap = ['viridis', 'plasma', 'inferno', 'magma', 'cividis', 'gray', 'purples', 'blues', 'greens', 'oranges', 'reds', 'spectral', 'coolwarm', 'bwr', 'seismic', 'twilight', 'hsv']

    for colormap in supported_colormap:

        node = session.createContainerNode(nodeName)
        node.setParameter('min_value', ll.Parameter(0.0))
        node.setParameter('max_value', ll.Parameter(1.0))
        node.setParameter('alpha', ll.Parameter(1.0))
        node.setParameter('reverse', ll.Parameter(False))
        node.setParameter('colormap', ll.Parameter(colormap))

        node.bind('in_image', in_image)
        node.init()

        out_rgba = node.getPort('out_rgba')
        assert(out_rgba is not None)
        assert(out_rgba.width == in_image.width)
        assert(out_rgba.height == in_image.height)
        assert(out_rgba.depth == in_image.depth)
        assert(out_rgba.channelType == ll.ChannelType.Uint8)
        assert(out_rgba.channels == 4)

        session.run(node)

        assert(not session.hasReceivedVulkanWarningMessages())

if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
