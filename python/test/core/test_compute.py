import sys
sys.path.append('../build/python/lib.linux-x86_64-2.7')

import pytest
import numpy as np
import lluvia as ll


def test_compile():

    session = ll.Session()
    memory  = session.createMemory()

    shaderCode = """
    #version 450

    // default values for the local group size as well as
    // indexes for the specialization constants to override them.
    layout (
        local_size_x_id = 1, local_size_x = 1,
        local_size_y_id = 2, local_size_y = 1,
        local_size_z_id = 3, local_size_z = 1
    ) in;

    layout(binding = 0) buffer in0  {int A[]; };
    layout(binding = 1) buffer in1  {int B[]; };
    layout(binding = 2) buffer out0 {int C[]; };

    void main() {
        const uint index = gl_GlobalInvocationID.x;
        C[index] = A[index] + B[index];
    }
    """

    localLength = 32
    gridLength  = 4
    length      = gridLength * localLength

    dtype  = np.int32

    A_host = np.arange(0, length, dtype=dtype)
    B_host = np.arange(0, length, dtype=dtype)
    C_host = A_host + B_host

    A = memory.createBufferFromHost(A_host)
    B = memory.createBufferFromHost(B_host)
    C = memory.createBufferFromHost(np.zeros(length, dtype=dtype))

    node = session.compileComputeNode(shaderCode,
        ['Buffer', 'Buffer', 'Buffer'],
        localSize=(localLength,1,1),
        gridSize=(gridLength, 1, 1))
    
    node.bind(0, A)
    node.bind(1, B)
    node.bind(2, C)
    node.run()

    C_copy = C.toHost(dtype=dtype)
    print(C_copy)
    assert((C_copy == C_host).all())
