"""
    lluvia.core.command_buffer
    --------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport command_buffer

from buffer cimport Buffer
from buffer import  Buffer

from compute_node cimport ComputeNode
from compute_node import  ComputeNode

from image cimport Image
from image import  Image


cdef class CommandBuffer:
    
    def __cinit__(self):
        pass


    def __dealloc__(self):
        pass


    def begin(self):
        pass


    def end(self):
        pass


    def copyBuffer(self, Buffer src, Buffer dst):
        pass


    def copyBufferToImage(self, Buffer src, Image dst):
        pass


    def copyImageToBuffer(self, Image src, Buffer dst):
        pass


    def changeImageLayout(self, Image img, str newLayout):
        pass


    def run(self, ComputeNode node):
        pass


    def memoryBarrier(self):
        pass

        