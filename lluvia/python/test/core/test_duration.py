import pytest
import numpy as np


def test_create():

    import lluvia as ll

    session = ll.createSession(loadNodeLibrary=False)
    assert(session is not None)

    duration = session.createDuration()
    assert(duration is not None)

    commandBuffer = session.createCommandBuffer()
    assert(commandBuffer is not None)

    commandBuffer.begin()
    commandBuffer.durationStart(duration)
    # here can go any operation one wants to measure its runtime
    commandBuffer.durationEnd(duration)
    commandBuffer.end()

    session.run(commandBuffer)


if __name__ == "__main__":

    from config import add_lluvia_import_path
    add_lluvia_import_path()

    raise SystemExit(pytest.main([__file__]))
