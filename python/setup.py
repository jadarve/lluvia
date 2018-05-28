"""
    setup.py
    --------

    :copyright: 2018, Juan David Adarve Bermudez. See AUTHORS for more details.
    :license: Apache-2 license, see LICENSE for more details.
"""

import os
from distutils.core import setup, Extension
from Cython.Build import cythonize

VULKAN_SDK = os.environ['VULKAN_SDK']

# TODO: Vulkan SDK include path
incDirs = ['../core/include',
           os.path.join(VULKAN_SDK, 'include')]

# path to build
libDirs = ['../build/core/lib',
           os.path.join(VULKAN_SDK, 'lib')]


libs   = ['lluvia-core',
          'stdc++',
          'vulkan']

cflags = ['-std=c++14', '-fPIC']

cython_directives = {'embedsignature' : True}

def createExtension(name, sources):

    global incDirs
    global libDirs
    global libs

    ext = Extension(name,
                    sources=sources,
                    include_dirs=incDirs,
                    library_dirs=libDirs,
                    libraries=libs,
                    runtime_library_dirs=libs,
                    language='c++',
                    extra_compile_args=cflags)

    return ext

# list of extension modules
extensions = list()

#################################################
# PURE PYTHON PACKAGES
#################################################
py_packages = ['lluvia', 'lluvia.z_impl']

# package data include Cython .pxd files
package_data = {'lluvia.z_impl' : ['*.pxd']}

#################################################
# CYTHON EXTENSIONS
#################################################
GPUmodulesTable = [('lluvia.z_impl.session', ['lluvia/z_impl/session.pyx']),
                   ('lluvia.z_impl.memory' , ['lluvia/z_impl/memory.pyx'])]

for mod in GPUmodulesTable:
    extList = cythonize(createExtension(mod[0], mod[1]), compiler_directives=cython_directives)
    extensions.extend(extList)


# call distutils setup
setup(name='lluvia',
    version='0.0.1',
    author='Juan David Adarve Bermudez',
    author_email='juanda0718@gmail.com',
    maintainer='Juan David Adarve Bermudez',
    maintainer_email='juanda0718@gmail',
    url='https://github.com/jadarve/lluvia',
    description='A real-time computer vision engine',
    license='Apache-2',
    packages=py_packages,
    ext_modules=extensions,
    package_data=package_data)
