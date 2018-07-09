"""
    setup.py
    --------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import os
import numpy as np
from setuptools import setup
from distutils.core import Extension


from Cython.Build import cythonize

VULKAN_SDK = os.environ['VULKAN_SDK']

incDirs = ['../core/include',
           os.path.join(VULKAN_SDK, 'include'),
           np.get_include()]

# path to build
libDirs = ['../build/core/lib',
           os.path.join(VULKAN_SDK, 'lib')]


libs   = ['lluvia-core',
          'stdc++',
          'vulkan']

cflags = ['-std=c++14',
          '-fPIC',
          '-Wno-unused-function']

cython_directives = {'embedsignature' : True,
                     'infer_types' : True}

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

# list of extension modules
extensions = list()

#################################################
# PURE PYTHON PACKAGES
#################################################
py_packages = ['lluvia', 'lluvia.core', 'lluvia.core.impl']

# package data include Cython .pxd files
package_data = {'lluvia.core' : ['*.pxd']}

#################################################
# CYTHON EXTENSIONS
#################################################
GPUmodulesTable = [('lluvia.core.core_buffer'    , ['lluvia/core/core_buffer.pyx']),
                   ('lluvia.core.command_buffer' , ['lluvia/core/command_buffer.pyx']),
                   ('lluvia.core.compute_node'   , ['lluvia/core/compute_node.pyx']),
                   ('lluvia.core.image'          , ['lluvia/core/image.pyx']),
                   ('lluvia.core.io'             , ['lluvia/core/io.pyx']),
                   ('lluvia.core.memory'         , ['lluvia/core/memory.pyx']),
                   ('lluvia.core.program'        , ['lluvia/core/program.pyx']),
                   ('lluvia.core.session'        , ['lluvia/core/session.pyx']),]

for mod in GPUmodulesTable:
    extList = cythonize(createExtension(mod[0], mod[1]), compiler_directives=cython_directives)
    extensions.extend(extList)


# call distutils setup
setup(name='lluvia',
    version='0.0.1',
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
    setup_requires=['pytest-runner'],
    tests_require=['pytest', 'imageio']
    )
