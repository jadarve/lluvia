import pytest
import lluvia as ll

def test_session():

    session = ll.createSession(loadNodeLibrary = False)
    assert(session is not None)


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
