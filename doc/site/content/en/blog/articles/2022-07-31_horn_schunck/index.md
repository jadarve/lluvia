---
date: 2022-07-31
title: "Implementing the Horn and Schunck optical flow algorithm"
linkTitle: "Horn and Schunck optical flow algorithm."
description: "GPU implementation of Horn and Schunck's variational method for estimating optical flow."
author: Juan Adarve
resources:
- src: "**.{png,jpg}"
  title: "Image #:counter"
---

## Background

The [Horn and Schunck][1] variational method for computing optical flow is one of the seminal works in the field. In introduces the idea of using a global smoothness constrain on the estimated optical flow. This constrain helps the numerical solution to find a good flow estimate even in image regions with poor texture.

Let $\mathbf{Y}(x, y, k)$ be the image at point $(x, y)$ and time index $k$.

## Implementation

The figure below illustrates the pipeline implementing the algorithm:

```plantuml
@startuml
skinparam linetype ortho

state HS as "HornSchunck" {

  state in_gray <<inputPin>>

  state ImageProcessor
  state ImageNormalize_uint_C1

  state NI_1 as "NumericIteration 1"
  state NI_2 as "NumericIteration 2"
  state NI_3 as "NumericIteration 3"
  state NI_N as "NumericIteration N"
  
  in_gray -down-> ImageProcessor
  in_gray -down-> ImageNormalize_uint_C1
  
  
  ImageNormalize_uint_C1 -down-> ImageProcessor: in_gray_old
  
  ImageProcessor -down-> NI_1: in_image_params
  ImageProcessor -down-> NI_2
  ImageProcessor -down-> NI_3
  ImageProcessor -down-> NI_N: in_image_params
  
  NI_1 -> NI_2
  NI_2 -> NI_3
  NI_3 -> NI_N: ...
  NI_N -> NI_1: in_flow, used for next image iteration
  
  NI_N -down-> out_flow <<outputPin>>
  ImageNormalize_uint_C1 -down> out_gray <<outputPin>>
}

note top of HS
Parameters
----------
alpha : float. Defaults to 0.05.
    Regularization gain.

iterations : int. Defaults to 1.
    Number of iterations run to compute the optical flow.

float_precision : int. Defaults to ll.FloatPrecision.FP32.
    Floating point precision used accross the algorithm. The outputs out_gray
    and out_flow will be of this floating point precision.
end note
@enduml
```

The **`HornSchunck`** is a *ContainerNode* that instantiates several *ComputeNode* implementing the algorithm. In particular, the **`ImageProcessor`** node computes image parameters from the pair of images `in_gray` and `in_gray_old`. Those parameters are transfered to the instances of **`NumericIteration`** through `in_image_params`, organized as follows:

* `in_image_params.x`: X component of the image gradient
* `in_image_params.y`: Y component of the image gradient
* `in_image_params.z`: temporal derivative between `in_gray` and `in_gray_old`.
* `in_image_params.w`: gain for this pixel computed from image gradient and `alpha` parameter.

This packaging of the image parameters is convenient as all values are packed together in a singe RGBA pixel. The floating point precision of this, and the estimated optical flow is controlled by the `float_precision` parameter.

The **`NumericIteration`** node takes the image parameters and a prior estimation of the optical flow, `in_flow`, and computes the next iteration of the flow field. The algorithm requires several iterations for the estimated flow to be of acceptable quality. In the figure above, the last iteration is denoted as `NumericIteration_N` and it feeds its output back as input to the first one, as well as the output of the **`HornSchunck`** node. The number of iterations is controlled by the `iterations` parameter.

The code block below shows how to run a simple pipeline:

```plantuml
@startuml
skinparam linetype ortho

state RGBA2Gray
state HS as "HornSchunck"
state Flow2RGBA

RGBA2Gray -down-> HS: in_gray
HS -down-> Flow2RGBA: in_flow
@enduml
```

where **`RGBA2Gray`** converts an input RGBA image to gray scale, **`HornSchunck`** computes the optical flow, and **`Flow2RGBA`** converts the optical flow to color representation.

{{< tabpane >}}
{{< tab header="Python" lang="python">}}
import lluvia as ll
import lluvia.util as ll_util
import matplotlib.pyplot as plt

# read two images as numpy arrays
frame_0 = ll_util.readRGBA('path to first image...')
frame_1 = ll_util.readRGBA('path to second image...')

# global session and memory objects
session = ll.createSession()
memory = session.createMemory(ll.MemoryPropertyFlagBits.DeviceLocal)

# this is the input of the comple pipeline
in_rgba = memory.createImageViewFromHost(frame_0)

RGBA2Gray = session.createComputeNode('lluvia/color/RGBA2Gray')
RGBA2Gray.bind('in_rgba', in_rgba)
RGBA2Gray.init()
RGBA2Gray.run() # run the node immediately in order to populate out_gray with valid values

in_gray = RGBA2Gray.getPort('out_gray')

HornSchunck = session.createContainerNode('lluvia/opticalflow/HornSchunck/HornSchunck')
HornSchunck.setParameter('alpha', ll.Parameter(0.05))
HornSchunck.setParameter('iterations', ll.Parameter(1000))
HornSchunck.setParameter('float_precision', ll.Parameter(ll.FloatPrecision.FP32.value))
HornSchunck.bind('in_gray', in_gray)

# when the node is initialized, it transfers the content of in_gray to out_gray.
HornSchunck.init()

out_gray = HornSchunck.getPort('out_gray')
out_flow = HornSchunck.getPort('out_flow')

flow2RGBA = session.createComputeNode('lluvia/viz/Flow2RGBA')
flow2RGBA.setParameter('max_flow', ll.Parameter(float(2)))
flow2RGBA.bind('in_flow', out_flow)
flow2RGBA.init()

out_flow_rgba = flow2RGBA.getPort('out_rgba')

duration = session.createDuration()

cmdBuffer = session.createCommandBuffer()
cmdBuffer.begin()
cmdBuffer.run(RGBA2Gray)
cmdBuffer.memoryBarrier()
cmdBuffer.durationStart(duration)
cmdBuffer.run(HornSchunck)
cmdBuffer.memoryBarrier()
cmdBuffer.durationEnd(duration)
cmdBuffer.run(flow2RGBA)
cmdBuffer.end()

# copy the content of the second frame to the in_rgba image before running the whole pipeline
in_rgba.fromHost(frame_1)

# run the pipeline
session.run(cmdBuffer)

# print runtime in milliseconds
print('{0:.02f} ms'.format(duration.nanoseconds / 1e6))

fig = plt.figure(figsize=(10, 6)); fig.set_tight_layout(True)
plt.subplot2grid((1,2), (0, 0)); plt.imshow(out_gray.toHost(), vmin=0, vmax=1, cmap='gray')
plt.subplot2grid((1,2), (0, 1)); plt.imshow(out_flow_rgba.toHost())
plt.show()
{{< /tab >}}
{{< /tabpane >}}

## Evaluation on the Middlebury dataset

The [Middlebury optical flow dataset][2] from [*Baker et. al.*][3] provides several real-life and synthetic image sequences with ground truth optical flow. The figures below shows the estimated optical flow for the test sequences whose ground truth is available.

The Horn ans Schunck algorithm is not well suited for large pixel displacements in between images. Considering this, the input images are scaled to half before entering the compute pipeline. The ground truth flow is scaled accordingly in order to be compared with the estimated flow. The *Endpoint Error* measures the different in magnitude between the ground truth and the estimation, it is computed as:

$$
EE = \sqrt{(u - u_\text{gt})^2 + (v - v_\text{gt})^2}
$$

The algorithm is configured as follows:

* `alpha`: 15.0/255
* `iterations`: 2000
* `float_precision`: FP32

In general, the estimated optical flow yields acceptable results in image regions with small displacements (e.g. Dimetrodon, Grove2, Hydrangea, and RubberWhale). In image regions with large displacements, the method is not able to compute a good results, as can be visualized in the Urban2 and Urban3 sequences.

**TODO: run in the 1080**
The results reported in this post were run on a Razer Blade 2021 Laptop equipped with an Nvidia RTX3070 GPU. The runtime is reported in the title of each figure, and is lower than 10 milliseconds for most of the image sequences. Section [Runtime performance](#runtime-performance).

{{< imgproc Dimetrodon Resize "2160x360" >}}
{{< /imgproc >}}
{{< imgproc Grove2 Resize "2160x360" >}}
{{< /imgproc >}}
{{< imgproc Grove3 Resize "2160x360" >}}
{{< /imgproc >}}
{{< imgproc Hydrangea Resize "2160x360" >}}
{{< /imgproc >}}
{{< imgproc RubberWhale Resize "2160x360" >}}
{{< /imgproc >}}
{{< imgproc Urban2 Resize "2160x360" >}}
{{< /imgproc >}}
{{< imgproc Urban3 Resize "2160x360" >}}
{{< /imgproc >}}
{{< imgproc Venus Resize "2160x360" >}}
{{< /imgproc >}}


## Runtime performance

Evaluate the performance in:

* Resolutions: 4K, UHD, FHD, HD, SD
* Float precision: FP16, FP32
* Fixed number of iteratios

## Discussion

## References

* Horn, Berthold KP, and Brian G. Schunck. "Determining optical flow." Artificial intelligence 17.1-3 (1981): 185-203. [Google Scholar][1].
* Baker, S., Scharstein, D., Lewis, J.P., Roth, S., Black, M.J. and Szeliski, R., 2011. A database and evaluation methodology for optical flow. International journal of computer vision, 92(1), pp.1-31. [Google Scholar][3].

[1]: https://scholar.google.com/citations?view_op=view_citation&hl=en&user=uWsBKQ0AAAAJ&citation_for_view=uWsBKQ0AAAAJ:u5HHmVD_uO8C
[2]: https://vision.middlebury.edu/flow/
[3]: https://scholar.google.com/scholar?cluster=5697474256105237450&hl=en&as_sdt=0,5
