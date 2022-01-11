import sys
import pytest
import numpy as np
import lluvia as ll
import lluvia.util as ll_util

from rules_python.python.runfiles import runfiles

def test_goodUse():

    r = runfiles.Create()
    programPath = r.Rlocation('lluvia/lluvia/nodes/lluvia/color/RGBA2Gray.comp.spv')
    scriptPath = r.Rlocation('lluvia/lluvia/nodes/lluvia/color/RGBA2Gray.lua')
    nodeName = 'lluvia/color/RGBA2Gray'
    programName = 'lluvia/color/RGBA2Gray.comp'

    session = ll.createSession(enableDebug=True, loadNodeLibrary=False)
    memory = session.createMemory(flags=[ll.MemoryPropertyFlagBits.DeviceLocal], pageSize=0)

    program = session.createProgram(programPath)
    session.setProgram(programName, program)
    session.scriptFile(scriptPath)
    
    node = session.createComputeNode(nodeName)

    imgRGBA = ll_util.readRGBA('lluvia/resources/mouse.jpg')
    in_rgba = memory.createImageViewFromHost(imgRGBA)

    node.bind('in_rgba', in_rgba)
    node.init()

    out_gray = node.getPort('out_gray')
    assert(out_gray is not None)
    assert(out_gray.width == in_rgba.width)
    assert(out_gray.height == in_rgba.height)
    assert(out_gray.depth == in_rgba.depth)
    assert(out_gray.channelType == ll.ChannelType.Uint8)
    assert(out_gray.channels == 1)

    session.run(node)

    assert(not ll.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
