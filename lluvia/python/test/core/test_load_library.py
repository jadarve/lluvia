
import pytest


def test_load_library():

    import lluvia as ll

    print("LL_PATH: ", ll.__path__)

    session = ll.createSession(loadNodeLibrary = False)
    session.loadLibrary('lluvia/cpp/core/test/nodes/test_node_library.zip')

    desc = session.createComputeNodeDescriptor('nodes/Assign')
    assert(desc != None)

    program = session.getProgram('nodes/Assign.comp')
    assert(program != None)

    node = session.createComputeNode("nodes/Assign")
    assert(node != None)


if __name__ == "__main__":

    from config import add_lluvia_import_path
    add_lluvia_import_path()

    raise SystemExit(pytest.main([__file__]))
