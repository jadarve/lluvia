"""
    lluvia.core.impl.enum_utils
    ---------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

from libc.stdint cimport uint32_t


cpdef uint32_t flattenFlagBits(flagBits, expectedType):
    """
    """

    cdef uint32_t flagsOR = 0

    if __isIterable(flagBits):
        for i, f in enumerate(flagBits):
            if type(f) is not expectedType:
                msg = 'Expecting type {0} at position {1}, got: {2}'
                msg = msg.format(str(expectedType),
                           i,
                           str(type(f)))
                raise ValueError(msg)

            flagsOR |= f
    else:
        if type(flagBits) is not expectedType:
            raise ValueError('Expecting type {0}, got: {1}'.format(str(expectedType),
                                                                   str(type(flagBits))))
        flagsOR |= flagBits

    return flagsOR


cpdef list expandFlagBits(uint32_t flags, flagBits):
    """
    """

    return [f for f in flagBits if (flags & f) == f]


def __isIterable(obj):
    """
    Checks if an object is iterable.

    Parameters
    ----------
    obj : the object

    Returns
    -------
    True if obj is iterable, False otherwise
    """

    try:
        iter(obj)
        return True
    except TypeError:
        return False
