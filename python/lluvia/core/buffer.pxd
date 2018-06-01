"""
    lluvia.core.buffer
    ------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t

from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.vector cimport vector
from libcpp.string cimport string

cimport vulkan as vk

cdef extern from 'lluvia/core/Buffer.h' namespace 'll':

    vk.BufferUsageFlags vectorStringToBufferUsageFLags(const vector[string]& flagsVector)
    vector[string] bufferUsageFlagsToVectorString(const vk.BufferUsageFlags flags)


    cdef cppclass _Buffer 'll::Buffer':

        uint64_t getSize() const
        vk.BufferUsageFlags getUsageFlags() const
        bool isMappable() const
        

cdef class Buffer:
    
    cdef shared_ptr[_Buffer] __buffer
