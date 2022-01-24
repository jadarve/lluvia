#!/usr/bin/env python3

"""
opticalFlow
-----------

Optical flow filter demo.

Usage
-----

./opticalFlow.py \
    --levels=4 \
    --max_flow=10 \
    <INPUT_VIDEO>

:copyright: 2020, Juan David Adarve. See AUTHORS for more details.
:license: Apache 2, see LICENSE for more details.
"""

import argparse
import cv2
import lluvia as ll


def parseArguments():

    parser = argparse.ArgumentParser(description='Optical flow filter.')
    parser.add_argument('input_file', type=str,
                        help='input video file path.')
    parser.add_argument('--levels', type=int, default=2,
                        help='Number of pyramid levels.')
    parser.add_argument('--max_flow', type=int, default=4,
                        help='Maximum optical flow allowed.')
    parser.add_argument('--gamma', type=float, default=0.0005,
                        help='Gain value for top level filter.')
    parser.add_argument('--gamma_low', type=float, default=0.0005,
                        help='Gain value for lowe levels filters.')
    parser.add_argument('--smooth_iterations', type=int, default=4,
                        help='Smooth iterations applied.')

    return parser.parse_args()


def main():

    args = parseArguments()

    ###########################################################################
    session = ll.createSession()
    memory = session.createMemory([ll.MemoryPropertyFlagBits.DeviceLocal])

    ###########################################################################
    cap = cv2.VideoCapture(args.input_file)
    r, img = cap.read()
    print('IMAGE SHAPE: ', img.shape)

    if not r:
        print('Error reading first frame')
        exit(-1)

    RGBA = cv2.cvtColor(img, cv2.COLOR_BGR2RGBA)
    in_rgba = memory.createImageViewFromHost(RGBA)

    ###########################################################################
    RGBA2Gray = session.createComputeNode('lluvia/color/RGBA2Gray')
    RGBA2Gray.bind('in_rgba', in_rgba)
    RGBA2Gray.init()

    flowFilter = session.createContainerNode('lluvia/opticalflow/flowfilter/FlowFilter')
    flowFilter.setParameter('levels', ll.Parameter(args.levels))
    flowFilter.setParameter('max_flow', ll.Parameter(args.max_flow))
    flowFilter.setParameter('smooth_iterations', ll.Parameter(args.smooth_iterations))
    flowFilter.setParameter('gamma', ll.Parameter(args.gamma))
    flowFilter.setParameter('gamma_low', ll.Parameter(args.gamma_low))
    flowFilter.bind('in_gray', RGBA2Gray.getPort('out_gray'))
    flowFilter.init()

    flow2RGBA = session.createComputeNode('lluvia/viz/Flow2RGBA')
    flow2RGBA.setParameter('max_flow', ll.Parameter(args.max_flow))
    flow2RGBA.bind('in_flow', flowFilter.getPort('out_flow'))
    flow2RGBA.init()

    ###########################################################################
    duration = session.createDuration()

    cmdBuffer = session.createCommandBuffer()
    cmdBuffer.begin()
    cmdBuffer.run(RGBA2Gray)
    cmdBuffer.memoryBarrier()

    # Duration is recorded for the actual algorithm execution, not the input/output conversions
    cmdBuffer.durationStart(duration)
    cmdBuffer.run(flowFilter)
    cmdBuffer.memoryBarrier()
    cmdBuffer.durationEnd(duration)

    cmdBuffer.run(flow2RGBA)
    cmdBuffer.end()

    width = in_rgba.width
    height = in_rgba.height

    while r:

        r, img = cap.read()
        if not r:
            break

        RGBA = cv2.cvtColor(img, cv2.COLOR_BGR2RGBA)

        in_rgba.fromHost(RGBA)
        session.run(cmdBuffer)

        out_rgba = flow2RGBA.getPort('out_rgba').toHost()
        flowBGR = cv2.cvtColor(out_rgba, cv2.COLOR_RGBA2BGR)

        ms = duration.nanoseconds * 1e-6

        img = cv2.putText(img, '{0:.03f} ms'.format(ms),
                          (width - 200, 30),
                          cv2.FONT_HERSHEY_SIMPLEX,
                          fontScale=1, color=(0, 0, 255), thickness=2)

        flowBGR = cv2.putText(flowBGR, '{0:.03f} ms'.format(ms),
                              (width - 200, 30),
                              cv2.FONT_HERSHEY_SIMPLEX,
                              fontScale=1, color=(0, 0, 255), thickness=2)

        cv2.imshow('img', img)
        cv2.imshow('flow', flowBGR)
        cv2.waitKey(30)


if __name__ == '__main__':
    main()
