"""
    lluvia.core.session
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cdef extern from 'vulkan/vulkan.hpp' namespace 'vk':

    cdef cppclass MemoryPropertyFlags 'vk::MemoryPropertyFlags':
        pass
