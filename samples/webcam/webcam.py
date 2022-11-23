#!/usr/bin/env python3

"""
webcam
-----------

Run a lluvia pipeline feeding images from a webcam.

Usage
-----

./webcam.py script_file node

Example
-------

./webcam.py --width=320 --height=240 scripts/horn_schunck.lua webcam/HornSchunck

:copyright: 2022, Juan David Adarve. See AUTHORS for more details.
:license: Apache 2, see LICENSE for more details.
"""

import argparse
import cv2
import lluvia as ll


def parseArguments():
    parser = argparse.ArgumentParser(description='Optical flow filter.')
    parser.add_argument('--width', type=int, default=640, help='Camera width.')
    parser.add_argument('--height', type=int, default=480, help='Camera height.')
    parser.add_argument('script_file', type=str,
                        help='Script file declaring the node to run.')
    parser.add_argument('node', type=str, help='the name of the node to run')

    return parser.parse_args()


def main():

    args = parseArguments()

    ###########################################################################
    session = ll.createSession()
    session.scriptFile(args.script_file)

    devMemory = session.createMemory([ll.MemoryPropertyFlagBits.DeviceLocal])

    ###########################################################################
    cap = cv2.VideoCapture(0)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, args.width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, args.height)
    
    r, img = cap.read()
    print('IMAGE SHAPE: ', img.shape)

    if not r:
        print('Error reading first frame')
        exit(-1)

    BGRA = cv2.cvtColor(img, cv2.COLOR_BGR2BGRA)
    in_bgra = devMemory.createImageViewFromHost(BGRA)

    ###########################################################################
    node = session.createContainerNode(args.node)
    node.bind('in_image', in_bgra)
    node.init()

    ###########################################################################
    duration = session.createDuration()

    cmdBuffer = session.createCommandBuffer()
    cmdBuffer.begin()
    cmdBuffer.durationStart(duration)
    cmdBuffer.run(node)
    cmdBuffer.durationEnd(duration)
    cmdBuffer.end()

    width = in_bgra.width

    ###########################################################################
    while r:

        r, img = cap.read()
        if not r:
            break

        BGRA = cv2.cvtColor(img, cv2.COLOR_BGR2BGRA)

        in_bgra.fromHost(BGRA)
        session.run(cmdBuffer)

        out_image = node.getPort('out_image').toHost()
        
        ms = duration.nanoseconds * 1e-6

        img = cv2.putText(img, '{0:.03f} ms'.format(ms),
                          (width - 200, 30),
                          cv2.FONT_HERSHEY_SIMPLEX,
                          fontScale=1, color=(0, 0, 255), thickness=2)

        out_image = cv2.putText(out_image, '{0:.03f} ms'.format(ms),
                              (width - 200, 30),
                              cv2.FONT_HERSHEY_SIMPLEX,
                              fontScale=1, color=(0, 0, 255), thickness=2)

        cv2.imshow('in_image', img)
        cv2.imshow('out_image', out_image)
        cv2.waitKey(30)
    
    cap.release()
    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
