"""
    lluvia.core.node.push_constants
    -------------------------------

    :copyright: 2022, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport int32_t, uint64_t


cdef extern from 'lluvia/core/node/PushConstants.h' namespace 'll':

    cdef cppclass _PushConstants:

        _PushConstants()
        _PushConstants(const _PushConstants&)

        uint64_t getSize() const

        void setFloat(const float&)
        float getFloat() const

        void setInt32(const int32_t&)
        float getInt32() const

