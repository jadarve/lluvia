## Setup

### OpenCV

```bash
mkdir -p ~/local/opencv && cd ~/local/opencv

# Download and unpack sources
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.5.2.zip
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.5.2.zip

unzip opencv.zip
unzip opencv_contrib.zip

mv opencv-4.5.2 opencv
mv opencv_contrib-4.5.2 opencv_contrib

# remove archives
rm opencv.zip
rm opencv_contrib.zip

# Create build directory and switch into it
mkdir -p opencv/build && cd opencv/build

cmake -D CMAKE_BUILD_TYPE=RELEASE \
-D CMAKE_INSTALL_PREFIX=/usr/local \
-D OPENCV_EXTRA_MODULES_PATH=~/local/opencv/opencv_contrib/modules \
-D WITH_OPENMP=ON \
-D WITH_OPENCL=OFF \
-D BUILD_ZLIB=ON \
-D BUILD_TIFF=ON \
-D WITH_FFMPEG=ON \
-D WITH_TBB=ON \
-D BUILD_TBB=ON \
-D BUILD_TESTS=OFF \
-D WITH_EIGEN=OFF \
-D WITH_GSTREAMER=ON \
-D WITH_V4L=ON \
-D WITH_LIBV4L=ON \
-D WITH_VTK=OFF \
-D WITH_QT=OFF \
-D OPENCV_ENABLE_NONFREE=ON \
-D INSTALL_C_EXAMPLES=OFF \
-D INSTALL_PYTHON_EXAMPLES=OFF \
-D BUILD_opencv_python3=TRUE \
-D OPENCV_GENERATE_PKGCONFIG=ON \
-D BUILD_EXAMPLES=OFF ..

make -j

sudo make install
```

## Optical Flow Example

First, build the node library target in the `lluvia` project:

```bash
cd ..
bazel build //lluvia/nodes:lluvia_node_library
```

Then, run the example:

```bash
# back to this folder
cd examples

# run the example
bazel run //opticalFlowCpp:opticalFlowCpp $(pwd)/../bazel-bin/lluvia/nodes/lluvia_node_library.zip
```

where `$(pwd)/../bazel-bin/lluvia/nodes/lluvia_node_library.zip` is the absolute path to the node library target archive containing the GLSL and Lua code that make the nodes.
