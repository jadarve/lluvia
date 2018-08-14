"""
    flowfilter.image_pyramid
    ------------------------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import lluvia as ll


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

		# memory to allocate all the pyramid images
		self.memory = self.session.createMemory('DeviceLocal')


		self.__initImages()
		sefl.__initComputeNodes()


	def record(self, cmdBuffer):
		pass


	def __initImages(self):

		self.imageOutput.append(self.inputImage)
		self.imageViewOutput.append(self.inputImage.createImageView('Nearest', 'Repeat', False, False))

		channels = self.inputImage.channels
		channelType = self.inputImage.channelType

		for h in range(1, self.levels):

			_, height, width, _ = self.imageOutput[h].shape

			imgX = self.memory.createImage((height, int(width/2)), channels, channelType)

			self.imageX.append(imgX)
			self.imageViewX.append(imgX.createImageView('Nearest', 'Repeat', False, False))

			imgY = self.memory.createImage((int(height/2), int(width/2)), channels, channelType)
			self.imageOutput.append(imgY)
			self.imageViewOutput.append(imgY.createImageView('Nearest', 'Repeat', False, False))


	def __initComputeNodes(self):

		if self.levels == 1:
			# nothing to compute, the outut of the pyramid is
			# self.imageOutput[0] and self.imageViewOutput[0]
			return


		programX = self.session.compileP
		programY

		prevImageView = self.imageViewOutput[0]
		for h in range(self.levels):

			pass