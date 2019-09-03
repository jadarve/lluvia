"""
    lluvia.core.impl.string_utils
    -----------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import sys

__all__ = ['isStringLike', 'encodeString']


def isStringLike(s):
    """
    Returns True if s is a string-like object.

    The condition to test is:

        type(s) is str or type(s) is bytes
    """

    return type(s) is str or type(s) is bytes


def encodeString(s):
    """
    Returns a string variable properly encoded
    to be used by the C++ API.

    Parameters
    ----------
    s : str or bytes.
        String to encode

    Returns
    -------
    encoded : str for Python 2, bytes for Python 3 and up.
        encoded string.
    """

    # Python 3 and up
    if sys.version_info.major >= 3:
        return bytes(s, 'utf-8') if type(s) is str else s

    # Python 2
    return s
