"""
    lluvia.core.command_buffer
    --------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport command_buffer

from cython.operator cimport dereference as deref

from buffer cimport Buffer
from buffer import  Buffer

from compute_node cimport ComputeNode
from compute_node import  ComputeNode

cimport image
from image cimport Image
from image import  Image, ImageLayout

import impl

cdef class CommandBuffer:
    
    def __cinit__(self):
        pass


    def __dealloc__(self):
        pass


    def begin(self):
        
        self.__commandBuffer.get().begin()


    def end(self):

        self.__commandBuffer.get().end()


    def copyBuffer(self, Buffer src, Buffer dst):

        self.__commandBuffer.get().copyBuffer(
            deref(src.__buffer.get()),
            deref(dst.__buffer.get()))


    def copyBufferToImage(self, Buffer src, Image dst):

        self.__commandBuffer.get().copyBufferToImage(
            deref(src.__buffer.get()),
            deref(dst.__image.get()))


    def copyImageToBuffer(self, Image src, Buffer dst):
        
        self.__commandBuffer.get().copyImageToBuffer(
            deref(src.__image.get()),
            deref(dst.__buffer.get()))


    def changeImageLayout(self, Image img, str newLayout):
        
        impl.validateFlagStrings(ImageLayout, newLayout)

        cdef vk.ImageLayout vkLayout = image.stringToImageLayout(newLayout)
        self.__commandBuffer.get().changeImageLayout(deref(img.__image.get()), vkLayout)


    def run(self, ComputeNode node):
        
        self.__commandBuffer.get().run(deref(node.__node))


    def memoryBarrier(self):
        
        self.__commandBuffer.get().memoryBarrier()

        