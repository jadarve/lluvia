
import pytest
import lluvia as ll

def test_load_library():

    session = ll.createSession(enableDebug=True, loadNodeLibrary = False)
    session.loadLibrary('lluvia/cpp/core/test/nodes/test_node_library.zip')

    desc = session.createComputeNodeDescriptor('nodes/Assign')
    assert(desc != None)

    program = session.getProgram('nodes/Assign.comp')
    assert(program != None)

    node = session.createComputeNode("nodes/Assign")
    assert(node != None)

    assert(not session.hasReceivedVulkanWarningMessages())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
