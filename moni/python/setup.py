import os
import numpy as np
from setuptools import setup
from distutils.core import Extension

from Cython.Build import cythonize

def createExtension(name, sources):

    global incDirs
    global libDirs
    global libs

    ext = Extension(name,
                    sources=sources,
                    include_dirs=incDirs,
                    library_dirs=libDirs,
                    libraries=libs,
                    runtime_library_dirs=libDirs,
                    language='c++',
                    extra_compile_args=cflags)

    return ext


VULKAN_SDK = os.environ['VULKAN_SDK'] if 'VULKAN_SDK' in os.environ.keys() else ''

incDirs = [os.path.join(VULKAN_SDK, 'include'),
           np.get_include()]

# path to build
libDirs = [os.path.join(VULKAN_SDK, 'lib')]


libs = ['stdc++',
        'vulkan',
        'm']

cflags = ['-std=c++17',
          '-fPIC',
          '-Wno-unused-function']

cython_directives = {'embedsignature': True,
                     'infer_types': True}

py_packages = [
    'moni'
]

package_dir = {
    'moni': 'src/moni'
}
package_data = {
    'moni' : ['*.pxd']
}

#################################################
# CYTHON EXTENSIONS
#################################################
cythonModules = [
    ('moni.moni', ['src/moni/moni.pyx']),
]

# list of extension modules
extensions = list()
for mod in cythonModules:
    extList = cythonize(createExtension(
        mod[0], mod[1]), compiler_directives=cython_directives)
    extensions.extend(extList)


setup(name='moni',
      version="0.0.1",
      author='Juan David Adarve Bermudez',
      author_email='juanda0718@gmail.com',
      maintainer='Juan David Adarve Bermudez',
      maintainer_email='juanda0718@gmail.com',
      url='https://github.com/jadarve/lluvia',
      description='A real-time computer vision engine',
      license='Apache-2',
      packages=py_packages,
      ext_modules=extensions,
      package_data=package_data,
      package_dir=package_dir,
      setup_requires=['pytest-runner'],
      tests_require=['pytest', 'imageio']
      )
