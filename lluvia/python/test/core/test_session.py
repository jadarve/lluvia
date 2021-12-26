
import pytest
import sys
import os


def test_session():

    import lluvia as ll

    session = ll.createSession(loadNodeLibrary = False)
    assert(session is not None)


if __name__ == "__main__":
    
    from config import add_lluvia_import_path
    add_lluvia_import_path()

    raise SystemExit(pytest.main([__file__]))
