
import sys
import pytest

# import lluvia.core
import lluvia as ll

def test_session():

    print(ll.__dict__)
    print(ll.__path__)

    p = ll.Parameter(value=1.0)
    # session = ll.createSession()
    # memory = session.createMemory()

    # buf = memory.createBuffer(1024)
    # print(buf.size)


if __name__ == "__main__":
    sys.exit(pytest.main([__file__]))
