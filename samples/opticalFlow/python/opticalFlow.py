import importlib
import os
import sys

import cv2
import numpy as np
import matplotlib.pyplot as plt

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


def initFlowFilter(session, maxFlow):

    desc = session.createContainerNodeDescriptor('FlowFilter')
    flowFilter = session.createContainerNode(desc)
    flowFilter.setParameter('levels', ll.Parameter(3))
    flowFilter.setParameter('gamma', ll.Parameter(0.0005))
    flowFilter.setParameter('max_flow', ll.Parameter(maxFlow))
    flowFilter.setParameter('smooth_iterations', ll.Parameter(4))

    desc = session.createComputeNodeDescriptor('Flow2RGBA')
    flowToColor = session.createComputeNode(desc)
    flowToColor.setParameter('max_flow', ll.Parameter(maxFlow))

    return flowFilter, flowToColor

def main():

    lluviaBasePath = '/home/juan/workspace/git/lluvia'
    sys.path.append(os.path.join(
        lluviaBasePath, 'build/python/lib.linux-x86_64-3.6'))

    importLluvia(lluviaBasePath)

    # imgSize = (720, 360)
    imgSize = (1280, 720)

    session = initLluvia(lluviaBasePath)
    memory = session.createMemory()
    flowFilter, flowToColor = initFlowFilter(session, 8)

    in_rgba = None
    cmdBuffer = None
    

    cap = cv2.VideoCapture(0)
    r, img = cap.read()
    print('IMAGE SHAPE: ', img.shape)

    if r:
        img = cv2.resize(img, imgSize)
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

    else:
        print('Error reading first frame')
        return

    while r:

        r, img = cap.read()
        if not r:
            break

        img = cv2.resize(img, imgSize)
        RGBA = cv2.cvtColor(img, cv2.COLOR_BGR2RGBA)

        in_rgba.fromHost(RGBA)
        session.run(cmdBuffer)

        flowColor = flowToColor.getPort('out_rgba').toHost()
        flowBGR = cv2.cvtColor(flowColor, cv2.COLOR_RGBA2BGR)

        cv2.imshow('img', img)
        cv2.imshow('flow', flowBGR)
        cv2.waitKey(20)


if __name__ == '__main__':
    main()
