"""
    lluvia.util
    -----------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import math

__all__ = ['calculateGridSize']


def calculateGridSize(local, imgShape):
    """
    Calculates the grid size of a compute node given its local size and an image parameter.

    Parameters
    ----------
    local : 3-int tuple.
        local size of the compute node.

    imgShape : 4-int tuple.
        Image shape in (depth, height, width, channels) format.

    Returns
    -------
    grid : 3-int tuple.
        (X, Y, Z) grid size.
    """

    calc = lambda length, l: int(math.ceil(float(length) / float(l)))

    x, y, z = local
    depth, height, width, _ = imgShape

    return (calc(width  , x),
            calc(height , y),
            calc(depth  , z))
