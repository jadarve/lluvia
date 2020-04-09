#!/usr/bin/env python3

"""
opticalFlow
-----------

Optical flow filter demo.

Usage
-----

LL_PATH='<Lluvia root path>' ./opticalFlow.py \
    --levels=4 \
    --max_flow=10 \
    <INPUT_VIDEO>

:copyright: 2020, Juan David Adarve. See AUTHORS for more details.
:license: Apache 2, see LICENSE for more details.
"""

import argparse
import importlib
import os
import sys

import cv2

# Lluvia modules, they need to be imported at runtime.
ll = None
util = None


def importLluvia(basePath):

    global ll
    global util

    pythonPath = os.path.join(basePath, 'build/python/lib.linux-x86_64-3.6')
    sys.path.append(pythonPath)

    ll = importlib.import_module('lluvia')
    util = importlib.import_module('lluvia.util')


def initLluvia(basePath):

    glslPath = os.path.join(basePath, 'bazel-bin/samples/opticalFlow/glsl')
    luaPath = os.path.join(basePath, 'samples/opticalFlow/lua')

    session = ll.createSession()
    util.loadNodes(session, glslPath, luaPath)

    return session


def initFlowFilter(session, args):

    desc = session.createContainerNodeDescriptor('FlowFilter')
    flowFilter = session.createContainerNode(desc)
    flowFilter.setParameter('levels', ll.Parameter(args.levels))
    flowFilter.setParameter('gamma', ll.Parameter(args.gamma))
    flowFilter.setParameter('gamma_low', ll.Parameter(args.gamma_low))
    flowFilter.setParameter('max_flow', ll.Parameter(args.max_flow))
    flowFilter.setParameter('smooth_iterations',
                            ll.Parameter(args.smooth_iterations))

    desc = session.createComputeNodeDescriptor('Flow2RGBA')
    flowToColor = session.createComputeNode(desc)
    flowToColor.setParameter('max_flow', ll.Parameter(args.max_flow))

    return flowFilter, flowToColor


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

    lluviaBasePath = None
    try:
        lluviaBasePath = os.environ['LL_PATH']
    except KeyError:
        print('Error reading LL_PATH environ variable')
        exit(-1)

    sys.path.append(os.path.join(
        lluviaBasePath, 'build/python/lib.linux-x86_64-3.6'))

    importLluvia(lluviaBasePath)

    session = initLluvia(lluviaBasePath)
    memory = session.createMemory()
    flowFilter, flowToColor = initFlowFilter(session, args)

    cap = cv2.VideoCapture(args.input_file)
    r, img = cap.read()
    print('IMAGE SHAPE: ', img.shape)

    if not r:
        print('Error reading first frame')
        exit(-1)

    RGBA = cv2.cvtColor(img, cv2.COLOR_BGR2RGBA)

    in_rgba = memory.createImageFromHost(RGBA).createImageView()
    flowFilter.bind('in_rgba', in_rgba)
    flowFilter.init()

    flowToColor.bind('in_flow', flowFilter.getPort('out_flow'))
    flowToColor.init()

    cmdBuffer = session.createCommandBuffer()
    cmdBuffer.begin()
    flowFilter.record(cmdBuffer)
    cmdBuffer.memoryBarrier()
    cmdBuffer.run(flowToColor)
    cmdBuffer.end()

    while r:

        r, img = cap.read()
        if not r:
            break

        RGBA = cv2.cvtColor(img, cv2.COLOR_BGR2RGBA)

        in_rgba.fromHost(RGBA)
        session.run(cmdBuffer)

        flowColor = flowToColor.getPort('out_rgba').toHost()
        flowBGR = cv2.cvtColor(flowColor, cv2.COLOR_RGBA2BGR)

        cv2.imshow('img', img)
        cv2.imshow('flow', flowBGR)
        cv2.waitKey(30)


if __name__ == '__main__':
    main()
