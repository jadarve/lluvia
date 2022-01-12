import pytest
import numpy as np
import lluvia as ll

def test_compile():

    session = ll.createSession(loadNodeLibrary = False)
    memory = session.createMemory()

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
    gridLength = 4
    length = gridLength * localLength

    dtype = np.int32

    A_host = np.arange(0, length, dtype=dtype)
    B_host = np.arange(0, length, dtype=dtype)
    C_host = A_host + B_host

    A = memory.createBufferFromHost(A_host)
    B = memory.createBufferFromHost(B_host)
    C = memory.createBufferLike(A_host)

    ports = [
        ll.PortDescriptor(0, 'in_A', ll.PortDirection.In, ll.PortType.Buffer),
        ll.PortDescriptor(1, 'in_B', ll.PortDirection.In, ll.PortType.Buffer),
        ll.PortDescriptor(2, 'out_C', ll.PortDirection.Out, ll.PortType.Buffer)
    ]

    node = session.compileComputeNode(ports,
                                      shaderCode,
                                      'main',
                                      localSize=(localLength, 1, 1),
                                      gridSize=(gridLength, 1, 1))

    node.bind('in_A', A)
    node.bind('in_B', B)
    node.bind('out_C', C)
    node.init()
    node.run()

    C_copy = C.toHost(dtype=dtype)
    assert((C_copy == C_host).all())


if __name__ == "__main__":

    raise SystemExit(pytest.main([__file__]))
