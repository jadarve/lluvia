"""
    lluvia.core.session
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint64_t

from libcpp cimport bool
from libcpp.memory cimport unique_ptr
from libcpp.memory cimport shared_ptr

from memory cimport _Memory

cimport vulkan as vk

cdef extern from 'lluvia/core/Session.h' namespace 'll':

    cdef cppclass _Session 'll::Session':

        @staticmethod
        unique_ptr[_Session] create()

        shared_ptr[_Memory] createMemory(const vk.MemoryPropertyFlags flags, const uint64_t pageSize, bool exactFlagsMatch = false) const


cdef class Session:
    
    cdef unique_ptr[_Session] __session

