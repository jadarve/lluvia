"""
    lluvia.util
    -----------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import math
import os
import glob
import imageio
import numpy as np


__all__ = [
    'calculateGridSize',
    'loadNodes',
    'readRGBA',
    'readSampleImage'
]


def loadNodes(session, glslPath, luaPath):
    """
    Load GLSL and Lua nodes into a session.

    Parameters
    ----------
    session : Session.
        Session where the nodes will be loaded.
    glslPath : str.
        Path where the SPIR-V files can be found.
    luaPath : std.
        Path where the Lua files can be found.
    """

    shaders = glob.glob(os.path.join(glslPath, '*.spv'))
    luas = glob.glob(os.path.join(luaPath, '*.lua'))

    for spirv in shaders:
        fname = os.path.split(spirv)[-1]
        programName = os.path.splitext(fname)[0]
        session.setProgram(programName, session.createProgram(spirv))

    for lua in luas:
        session.scriptFile(lua)


def readRGBA(path):
    """
    Reads an image and converts it to RGBA.

    Parameters
    ----------
    path : str.
        Image path.

    Returns
    -------
    RGBA : np.ndarray.
        RGBA image.
    """

    img = imageio.imread(path)
    RGBA = np.zeros(img.shape[:-1] + tuple([4]), dtype=img.dtype)
    RGBA[..., :3] = img

    return RGBA

def readSampleImage(name):
    """
    Reads a sample image packed with Lluvia.

    The available images are:

    * mouse
    * koala

    Parameters
    ----------
    name : str
        The name of the image

    Returns
    -------
    RGBA : np.ndarray
        RGBA image
    """

    # From https://stackoverflow.com/questions/6028000/how-to-read-a-static-file-from-inside-a-python-package
    try:
        import importlib.resources as pkg_resources
    except ImportError:
        # Try backported to PY<37 `importlib_resources`.
        import importlib_resources as pkg_resources
    
    import lluvia.resources as rsc

    arr = pkg_resources.read_binary(rsc, name + '.jpg')

    imreader = imageio.get_reader(arr, '.jpg')
    img = imreader.get_data(0)

    RGBA = np.zeros(img.shape[:-1] + tuple([4]), dtype=img.dtype)
    RGBA[..., :3] = img

    return RGBA


def calculateGridSize(local, imgShape):
    """
    Calculates the grid size of a compute node given its local size
    and an image parameter.

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

    x, y, z = local
    depth, height, width, _ = imgShape

    return (__calculateGridAxis(width, x),
            __calculateGridAxis(height, y),
            __calculateGridAxis(depth, z))


def __calculateGridAxis(length, local):
    """
    Calculates the grid axis.

    Parameters
    ----------
    length : int.
        Global length.
    local : int.
        Local size.

    Returns
    -------
    grid : int
        Calculated grid size.

    """

    return int(math.ceil(float(length) / float(local)))
