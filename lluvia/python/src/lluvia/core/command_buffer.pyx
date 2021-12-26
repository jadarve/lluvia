
"""
    lluvia.core.command_buffer
    --------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""


from cython.operator cimport dereference as deref

from lluvia.core cimport vulkan as vk
from lluvia.core import impl
from lluvia.core.buffer.buffer cimport Buffer
from lluvia.core.duration cimport Duration

from lluvia.core.image.image_layout cimport _ImageLayout, ImageLayout
from lluvia.core.image.image cimport Image

from lluvia.core.node cimport ComputeNode


__all__ = ['CommandBuffer']


cdef _buildCommandBuffer(shared_ptr[_CommandBuffer] ptr, Session session):

    cdef CommandBuffer cmdBuffer = CommandBuffer()
    cmdBuffer.__commandBuffer = ptr
    cmdBuffer.__session = session

    return cmdBuffer


cdef class CommandBuffer:

    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    property session:
        def __get__(self):
            return self.__session

    def begin(self):
        """
        Begin recording.

        This method should be called before any record operation. Otherwise
        the behaviour is undefined.
        """

        self.__commandBuffer.get().begin()

    def end(self):
        """
        Ends recording.

        Any following call to recording methods after this call have undefined
        behavior.
        """

        self.__commandBuffer.get().end()

    def copyBuffer(self, Buffer src, Buffer dst):
        """
        Copies src buffer to dst.

        The parameters must satisfy `dst.size >= src.size`.

        Parameters
        ----------
        src : Buffer.
            Source buffer.

        dst : Buffer.
            Destination buffer.


        Raises
        ------
        RuntimeError : if dst.size < src.size
        """

        self.__commandBuffer.get().copyBuffer(
            deref(src.__buffer.get()),
            deref(dst.__buffer.get()))

    def copyBufferToImage(self, Buffer src, Image dst):
        """
        Copies the content of src Buffer to dst Image

        No size check is currently being tested.

        Parameters
        ----------
        src : Buffer.
            Source Buffer.

        dst : Image.
            Source Image.
        """

        self.__commandBuffer.get().copyBufferToImage(
            deref(src.__buffer.get()),
            deref(dst.__image.get()))

    def copyImageToBuffer(self, Image src, Buffer dst):
        """
        Copies the content of src Image to dst Buffer.

        Parameters
        ----------
        src : Image.
            Source image.

        dst : Buffer.
            Destination buffer.
        """

        self.__commandBuffer.get().copyImageToBuffer(
            deref(src.__image.get()),
            deref(dst.__buffer.get()))

    def copyImageToImage(self, Image src, Image dst):
        """
        Copies the content of src Image to dst Image.

        The image parameters must be in the following layouts:

        * src: TransferSrcOptimal
        * dst: TransferDstOptimal

        This method does not check if the images are in the correct
        layout. The results are undefined if the images are in
        any other layout.

        Parameters
        ----------
        src : Image.
            Source image.

        dst : Image.
            Destination buffer.
        """

        self.__commandBuffer.get().copyImageToImage(
            deref(src.__image.get()),
            deref(dst.__image.get()))

    def changeImageLayout(self, Image img, ImageLayout newLayout):
        """
        Changes image layout.

        The actual change in image layout is performed when this command buffer
        is submitted to execution. However, calling `image.getLayout()`
        right after this call will return the new layout value.

        This is necessary to keep track of the current image layout of
        \p image parameter after this call.


        Parameters
        ----------
        img : Image.
            The image.

        newLayout : lluvia.ImageLayout.
            The new layout. Its value must be one of the values
            defined in lluvia.ImageLayout:
                - Undefined
                - General
                - ShaderReadOnlyOptimal
                - TransferSrcOptimal
                - TransferDstOptimal
                - Preinitialized
        """

        cdef _ImageLayout _layout = <_ImageLayout> newLayout
        self.__commandBuffer.get().changeImageLayout(deref(img.__image.get()), _layout)

    def clearImage(self, Image img):
        """
        Clears the pixels of the image to zero.
        """

        self.__commandBuffer.get().clearImage(deref(img.__image.get()))

    def run(self, ComputeNode node):
        """
        Records running a compute node.

        This operation uses node.grid to dispatch the compute shader. After
        the call of this method, node.grid can be changed to a new value.

        Parameters
        ----------
        node : ComputeNode.
            The compute node.
        """

        self.__commandBuffer.get().run(deref(node.__node))

    def memoryBarrier(self):
        """
        Inserts a memory barrier.

        TODO: details.
        """

        self.__commandBuffer.get().memoryBarrier()

    def durationStart(self, Duration d):
        """
        Starts recording the elapsed time between two points.

        Parameters
        ----------
        d : Duration.
            The duration object.
        """

        self.__commandBuffer.get().durationStart(deref(d.__duration))

    def durationEnd(self, Duration d):
        """
        Stops recording the elapsed time between two points.

        Parameters
        ----------
        d : Duration.
            The duration object.
        """

        self.__commandBuffer.get().durationEnd(deref(d.__duration))
