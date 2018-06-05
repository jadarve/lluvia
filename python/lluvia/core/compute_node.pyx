"""
    lluvia.core.compute_node
    ------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

cimport compute_node

from buffer import Buffer
from buffer cimport Buffer

from program import Program
from program cimport Program, _Program

from impl.stdcpp cimport static_pointer_cast

from libc.stdint cimport uint32_t

from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

__all__ = ['ComputeNodeDescriptor', 'ParameterType']

ParameterType = ["Buffer", "ImageView", "SampledImageView"]


cdef class ComputeNodeDescriptor:

    def __cinit__(self):
        self.__descriptor = _ComputeNodeDescriptor()


    def __dealloc__(self):
        pass


    property program:
        def __get__(self):

            cdef Program prog = Program()
            prog.__program = self.__descriptor.getProgram()
            return prog

        def __set__(self, Program p):
            self.__descriptor.setProgram(p.__program)

        def __del__(self):
            pass


    property functionName:
        def __get__(self):
            return self.__descriptor.getFunctionName()

        def __set__(self, str functionName):
            self.__descriptor.setFunctionName(functionName)

        def __del__(self):
            pass


    property grid:
        def __get__(self):
            return (self.gridX, self.gridY, self.gridZ)

        def __set__(self, v):
            assert (len(v) == 3)
            self.gridX, self.gridY, self.gridZ = v

        def __del__(self):
            pass


    property local:
        def __get__(self):
            return (self.localX, self.localY, self.localZ)

        def __set__(self, v):
            assert (len(v) == 3)
            self.localX, self.localY, self.localZ = v

        def __del__(self):
            pass


    property gridX:
        def __get__(self):
            return self.__descriptor.getGridX()

        def __set__(self, uint32_t x):
            if x <= 0: raise ValueError('gridX must be greater than zero, got: {0}'.format(x))
            self.__descriptor.setGridX(x)

        def __del__(self):
            pass


    property gridY:
        def __get__(self):
            return self.__descriptor.getGridY()

        def __set__(self, uint32_t y):
            if y <= 0: raise ValueError('gridY must be greater than zero, got: {0}'.format(y))
            self.__descriptor.setGridY(y)

        def __del__(self):
            pass


    property gridZ:
        def __get__(self):
            return self.__descriptor.getGridZ()

        def __set__(self, uint32_t z):
            if z <= 0: raise ValueError('gridZ must be greater than zero, got: {0}'.format(z))
            self.__descriptor.setGridZ(z)

        def __del__(self):
            pass


    property localX:
        def __get__(self):
            return self.__descriptor.getLocalX()

        def __set__(self, uint32_t x):
            if x <= 0: raise ValueError('localX must be greater than zero, got: {0}'.format(x))
            self.__descriptor.setLocalX(x)

        def __del__(self):
            pass


    property localY:
        def __get__(self):
            return self.__descriptor.getLocalY()

        def __set__(self, uint32_t y):
            if y <= 0: raise ValueError('localY must be greater than zero, got: {0}'.format(y))
            self.__descriptor.setLocalY(y)

        def __del__(self):
            pass


    property localZ:
        def __get__(self):
            return self.__descriptor.getLocalZ()

        def __set__(self, uint32_t z):
            if z <= 0: raise ValueError('localZ must be greater than zero, got: {0}'.format(z))
            self.__descriptor.setLocalZ(z)

        def __del__(self):
            pass


    def addParameter(self, str paramType):
        """
        Adds a parameter to the descriptor.

        Parameters are stored in a vector. Each call to this function
        adds the new parameter at the end of the vector
    
        Parameters
        ----------
        paramType : string.
            Parameter type. It must be one of the values stored in lluvia.ParameterType.


        Raises
        ------
        ValueError : if paramType is not found in lluvia.ParameterType.
        """

        if paramType not in ParameterType:
            raise ValueError('Unknown parameter type \'{0}\', expecting one of: {1}'.format(paramType, ParameterType))

        cdef string cppString = paramType
        cdef _ParameterType pType = stringToParameterType(cppString)
        self.__descriptor.addParameter(pType)


cdef class ComputeNode:
    
    def __cinit__(self):
        pass


    def __dealloc__(self):
        pass


    property grid:
        def __get__(self):
            return (self.gridX, self.gridY, self.gridZ)

        # def __set__(self, v):
        #     assert (len(v) == 3)
        #     self.gridX, self.gridY, self.gridZ = v

        def __del__(self):
            pass


    property local:
        def __get__(self):
            return (self.localX, self.localY, self.localZ)

        def __del__(self):
            pass


    property gridX:
        def __get__(self):
            return self.__node.get().getGridX()

        # def __set__(self, uint32_t x):
        #     if x <= 0: raise ValueError('gridX must be greater than zero, got: {0}'.format(x))
        #     self.__node.get().setGridX(x)

        def __del__(self):
            pass


    property gridY:
        def __get__(self):
            return self.__node.get().getGridY()

        # def __set__(self, uint32_t y):
        #     if y <= 0: raise ValueError('gridY must be greater than zero, got: {0}'.format(y))
        #     self.__node.get().setGridY(y)

        def __del__(self):
            pass


    property gridZ:
        def __get__(self):
            return self.__node.get().getGridZ()

        # def __set__(self, uint32_t z):
        #     if z <= 0: raise ValueError('gridZ must be greater than zero, got: {0}'.format(z))
        #     self.__node.get().setGridZ(z)

        def __del__(self):
            pass


    property localX:
        def __get__(self):
            return self.__node.get().getLocalX()

        def __del__(self):
            pass


    property localY:
        def __get__(self):
            return self.__node.get().getLocalY()

        def __del__(self):
            pass


    property localZ:
        def __get__(self):
            return self.__node.get().getLocalZ()

        def __del__(self):
            pass


    def bind(self, uint32_t index, obj):
        """
        Binds an object as parater to this node.


        Parameters
        ----------
        index : uint32_t
            Index of the parameter to bind.

        obj : lluvia.Buffer or lluvia.ImageView
            Parameter to bind.
        """

        cdef Buffer buf = Buffer()
        
        if type(obj) == Buffer:
            buf = obj
            self.__node.get().bind(index, static_pointer_cast[_Object](buf.__buffer))
