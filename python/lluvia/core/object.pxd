"""
    lluvia.core.program
    -------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cdef extern from 'lluvia/core/Object.h' namespace 'll':

    cdef cppclass _Object 'll::Object':
        pass
        