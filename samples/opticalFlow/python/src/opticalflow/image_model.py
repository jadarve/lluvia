"""
    opticalflow.image_model
    -----------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import lluvia as ll

shaderCode = """
#version 450

#include <lluvia/core.glsl>

layout(binding = 0, r8ui) uniform uimage2D inputImage;
layout(binding = 1, rg32f) uniform image2D outputImage;

void main() {

    // this coordinates are relative to outputImage size
    const ivec2 coords  = LL_GLOBAL_COORDS_2D;
    const ivec2 imgSize = imageSize(outputImage);

    if (coords.x > imgSize.x || coords.y > imgSize.y) {
        return;
    }

    const bool leftBorder   = coords.x == 0;
    const bool rightBorder  = coords.x == imgSize.x - 1;
    const bool topBorder    = coords.y == 0;
    const bool bottomBorder = coords.y == imgSize.y -1;

    // central pixel in the 3x3 window
    const uint img_11 = imageLoad(inputImage, coords).r;

    const uint img_00 = leftBorder || topBorder?     img_11 : imageLoad(inputImage, coords + ivec2(-1, -1)).r;
    const uint img_01 = topBorder?                   img_11 : imageLoad(inputImage, coords + ivec2( 0, -1)).r;
    const uint img_02 = rightBorder || topBorder?    img_11 : imageLoad(inputImage, coords + ivec2(+1, -1)).r;
    const uint img_10 = leftBorder?                  img_11 : imageLoad(inputImage, coords + ivec2(-1,  0)).r;
    const uint img_12 = rightBorder?                 img_11 : imageLoad(inputImage, coords + ivec2(+1,  0)).r;
    const uint img_20 = leftBorder || bottomBorder?  img_11 : imageLoad(inputImage, coords + ivec2(-1, +1)).r;
    const uint img_21 = bottomBorder?                img_11 : imageLoad(inputImage, coords + ivec2( 0, +1)).r;
    const uint img_22 = rightBorder || bottomBorder? img_11 : imageLoad(inputImage, coords + ivec2(+1, +1)).r;

    const float grad_x = (0.25*(img_02 + img_22) + 0.5*img_12) - (0.25*(img_00 + img_02) + 0.5*img_02);
    const float grad_y = (0.25*(img_20 + img_22) + 0.5*img_21) - (0.25*(img_00 + img_02) + 0.5*img_01);
    const float imgFiltered = 0.0625*(img_00 + img_02 + img_20 + img_22)
                            + 0.125*(img_01 + img_12 + img_21 + img_10)
                            + 0.25*img_11;

    // normalized image intensity in range [0, 1] and gradients in range [-1, 1]
    const vec4 outputValue = vec4(grad_x / 255, grad_y / 255, imgFiltered / 255, 0);

    imageStore(outputImage, coords, outputValue);
}
"""


class ImageModel(object):

    def __init__(self, **kwargs):
        """
        Parameters
        ----------
        session : lluvia.Session.
            Session from which to construct all the
            elements required for the pyramid.

        inputImage : lluvia.Image.
            Input image with format r8ui.
        """

        self.session    = kwargs['session']
        self.inputImage = kwargs['inputImage']

        self.__initImages()
        self.__initComputeNodes(kwargs['compileOptions'])


    def record(self, cmdBuffer):
        
        cmdBuffer.run(self.computeNode)
        cmdBuffer.memoryBarrier()


    def __initImages(self):

        self.inputImageView = self.inputImage.createImageView('Nearest', 'Repeat', False, False)

        # 4 channels floating point (4 bytes)
        self.memory = self.session.createMemory('DeviceLocal', 4*4*self.inputImage.size)
        
        _, height, width, _  = self.inputImage.shape
        self.outputImage     = self.memory.createImage((height, width, 4), 'float32')
        self.outputImageView = self.outputImage.createImageView('Nearest', 'Repeat', False, False)


    def __initComputeNodes(self, compileOptions):

        desc = ll.ComputeNodeDescriptor()
        desc.program      = self.session.compileProgram(shaderCode, includeDirs      = compileOptions['includeDirs'])
        desc.functionName = 'main'
        desc.local        = [32, 32, 1]
        desc.grid         = ll.calculateGridSize(desc.local, self.outputImage.shape)
        desc.addParameter('ImageView') # inputImageView
        desc.addParameter('ImageView') # outputImageView

        self.computeNode = self.session.createComputeNode(desc)
        self.computeNode.bind(0, self.inputImageView)
        self.computeNode.bind(1, self.outputImageView)