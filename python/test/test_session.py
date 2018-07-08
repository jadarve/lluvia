import pytest
import lluvia as ll


def test_session():
    
    session = ll.Session()
    memory = session.createMemory()

    print(memory.pageSize)    

    print('moni moni moni')
