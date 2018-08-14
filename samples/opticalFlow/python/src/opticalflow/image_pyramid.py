"""
    opticalflow.image_pyramid
    -------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import lluvia as ll


imgdownX_r8ui = """
#version 450

#include <lluvia/core.glsl>

layout(binding = 0, r8ui) uniform uimage2D inputImage;
layout(binding = 1, r8ui) uniform uimage2D outputImage;

void main() {

    // this coordinates are relative to outputImage size
    const ivec2 coords  = LL_GLOBAL_COORDS_2D;
    const ivec2 imgSize = imageSize(outputImage);

    if (coords.x > imgSize.x || coords.y > imgSize.y) {
        return;
    }

    const bool leftBorder  = coords.x == 0;
    const bool rightBorder = coords.x == imgSize.x - 1;

    // image coordinates for inputImage (twice the width of outputImage)
    const ivec2 inputImgCoords = ivec2(2*coords.x, coords.y);

    const uint img_0 = imageLoad(inputImage, inputImgCoords).r;

    const uint img_m = leftBorder  ? img_0 : imageLoad(inputImage, inputImgCoords + ivec2(-1, 0)).r;
    const uint img_p = rightBorder ? img_0 : imageLoad(inputImage, inputImgCoords + ivec2(+1, 0)).r;    

    // this is equivalent to 0.5*img_0 + 0.25*(img_m + img_p)
    const uint filterValue = (img_0 >> 1) + ((img_m + img_p) >> 2);

    imageStore(outputImage, coords, uvec4(filterValue));
}

"""

imgdownY_r8ui = """
#version 450

#include <lluvia/core.glsl>

layout(binding = 0, r8ui) uniform uimage2D inputImage;
layout(binding = 1, r8ui) uniform uimage2D outputImage;

void main() {

    // this coordinates are relative to outputImage size
    const ivec2 coords  = LL_GLOBAL_COORDS_2D;
    const ivec2 imgSize = imageSize(outputImage);

    if (coords.x > imgSize.x || coords.y > imgSize.y) {
        return;
    }

    const bool topBorder    = coords.y == 0;
    const bool bottomBorder = coords.y == imgSize.y -1;

    const ivec2 inputImgCoords = ivec2(coords.x, 2*coords.y);

    const uint img_0 = imageLoad(inputImage, inputImgCoords).r;

    const uint img_m = topBorder    ? img_0 : imageLoad(inputImage, inputImgCoords + ivec2(0, -1)).r;
    const uint img_p = bottomBorder ? img_0 : imageLoad(inputImage, inputImgCoords + ivec2(0, +1)).r;    

    // this is equivalent to 0.5*img_0 + 0.25*(img_m + img_p)
    const uint filterValue = (img_0 >> 1) + ((img_m + img_p) >> 2);

    imageStore(outputImage, coords, uvec4(filterValue));
}
"""


class ImagePyramid(object):

	def __init__(self, **kwargs):
		"""

		Parameters
		----------
		session : lluvia.Session.
			Session from which to construct all the
			elements required for the pyramid.

		inputImage : lluvia.Image.
			Input image.

		levels : int.
			Number of pyramid levels. It must be greater or equal 1.

		"""

		self.session    = kwargs['session']
		self.inputImage = kwargs['inputImage']
		self.levels     = kwargs['levels']

		# Computations are performed first in the X direction and then Y
		# The output of the pyramid are the images and image views in the
		# Y axis.
		self.imageX          = list()
		self.imageOutput     = list()
		self.imageViewX      = list()
		self.imageViewOutput = list()

		self.computeNodesX = list()
		self.computeNodesY = list()

		# memory to allocate all the pyramid images
		self.memory = self.session.createMemory('DeviceLocal')


		self.__initImages()
		self.__initComputeNodes(kwargs['compileOptions'])


	def record(self, cmdBuffer):
		
		if self.levels == 1:
			pass

		for h in range(self.levels - 1):

			cmdBuffer.run(self.computeNodesX[h])
			cmdBuffer.memoryBarrier()

			cmdBuffer.run(self.computeNodesY[h])
			cmdBuffer.memoryBarrier()


	def __initImages(self):

		self.imageOutput.append(self.inputImage)
		self.imageViewOutput.append(self.inputImage.createImageView('Nearest', 'Repeat', False, False))

		channels = self.inputImage.channels
		channelType = self.inputImage.channelType

		for h in range(1, self.levels):

			_, height, width, _ = self.imageOutput[h-1].shape

			imgX = self.memory.createImage((height, int(width/2)), channels, channelType)

			self.imageX.append(imgX)
			self.imageViewX.append(imgX.createImageView('Nearest', 'Repeat', False, False))

			imgY = self.memory.createImage((int(height/2), int(width/2)), channels, channelType)
			self.imageOutput.append(imgY)
			self.imageViewOutput.append(imgY.createImageView('Nearest', 'Repeat', False, False))


	def __initComputeNodes(self, compileOptions):

		if self.levels == 1:
			# nothing to compute, the outut of the pyramid is
			# self.imageOutput[0] and self.imageViewOutput[0]
			return

		programX = self.session.compileProgram(imgdownX_r8ui, includeDirs=compileOptions['includeDirs'])
		programY = self.session.compileProgram(imgdownY_r8ui, includeDirs=compileOptions['includeDirs'])

		descX = ll.ComputeNodeDescriptor()
		descX.program = programX
		descX.functionName = 'main'
		descX.local = [32, 32, 1]
		descX.grid = [1, 1, 1]
		descX.addParameter('ImageView')
		descX.addParameter('ImageView')

		descY = ll.ComputeNodeDescriptor()
		descY.program = programY
		descY.functionName = 'main'
		descY.local = [32, 32, 1]
		descY.grid = [1, 1, 1]
		descY.addParameter('ImageView')
		descY.addParameter('ImageView')

		prevImageView = self.imageViewOutput[0]

		for h in range(self.levels -1):

			imgViewX     = self.imageViewX[h]
			imgViewY_in  = self.imageViewOutput[h]
			imgViewY_out = self.imageViewOutput[h + 1]

			print('level: {0}'.format(h))
			print('\timgViewX.shape: {0}'.format(imgViewX.shape))
			print('\timgViewY_in.shape: {0}'.format(imgViewY_in.shape))
			print('\timgViewY_out.shape: {0}'.format(imgViewY_out.shape))

			compX = self.session.createComputeNode(descX)
			compX.grid = ll.calculateGridSize(compX.local, imgViewX.shape)
			compX.bind(0, imgViewY_in) # input
			compX.bind(1, imgViewX)    # output
			print('\tcompX.grid: {0}'.format(compX.grid))

			compY = self.session.createComputeNode(descY)
			compY.grid = ll.calculateGridSize(compY.local, imgViewY_out.shape)
			compY.bind(0, imgViewX)     # input
			compY.bind(1, imgViewY_out)	# output
			print('\tcompY.grid: {0}'.format(compY.grid))

			self.computeNodesX.append(compX)
			self.computeNodesY.append(compY)

