"""
These symbols are used in the BUILD.bazel to build the node library.
They can also be used by external repositories to build the library
as part of their targets.

This is needed for lluvia-mediapipe to embed the library as part of
the Android archive target.
"""

LLUVIA_NODE_LIBRARY_STRIP_PREFIX = "/lluvia/nodes"
LLUVIA_NODE_LIBRARY_NODES = [
    "@lluvia//lluvia/nodes/lluvia:Sobel",
    "@lluvia//lluvia/nodes/lluvia/camera:CameraUndistort_rgba8ui",
    "@lluvia//lluvia/nodes/lluvia/color:BGRA2Gray",
    "@lluvia//lluvia/nodes/lluvia/color:HSVA2RGBA",
    "@lluvia//lluvia/nodes/lluvia/color:RGBA2BGRA",
    "@lluvia//lluvia/nodes/lluvia/color:RGBA2Gray",
    "@lluvia//lluvia/nodes/lluvia/color:RGBA2HSVA",
    "@lluvia//lluvia/nodes/lluvia/imgproc:ImageDownsampleX_r8ui",
    "@lluvia//lluvia/nodes/lluvia/imgproc:ImageDownsampleY_r8ui",
    "@lluvia//lluvia/nodes/lluvia/imgproc:ImagePyramid_r8ui",
    "@lluvia//lluvia/nodes/lluvia/math/normalize:ImageNormalize_uint_C1",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowFilter",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowFilterDelta",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowFilterSimple",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowPredict",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowPredictPayload",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowPredictPayloadX",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowPredictPayloadY",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowPredictX",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowPredictY",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowSmooth",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowUpdate",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:FlowUpdateDelta",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/flowfilter:ImageModel",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/HornSchunck:HornSchunck",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/HornSchunck:ImageProcessor",
    "@lluvia//lluvia/nodes/lluvia/opticalflow/HornSchunck:NumericIteration",
    "@lluvia//lluvia/nodes/lluvia/viz:Flow2RGBA",
    "@lluvia//lluvia/nodes/lluvia/viz/colormap:ColorMap_float",
    "@lluvia//lluvia/nodes/lluvia/viz/colormap:ColorMap_int",
    "@lluvia//lluvia/nodes/lluvia/viz/colormap:ColorMap_uint",
    "@lluvia//lluvia/nodes/lluvia/viz/colormap:ColorMap",
]
