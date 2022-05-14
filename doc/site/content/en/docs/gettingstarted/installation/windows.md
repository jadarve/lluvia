---
title: "Windows 10"
date: 2021-06-09T00:00:00-05:00
weight: 3
---

## Dependencies

* Python2. This is needed for Bazel to be able to run Python binaries

    ```powershell
    choco install python2

    python -m pip install jinja2
    ```

* Python3 dependencies

    ```powershell
    pip3 install cython numpy pytest jinja2 markupsafe
    ```

* Vulkan SDK: follow the official installation instructions from [LunarG](https://vulkan.lunarg.com/sdk/home#windows).

* Bazel: follow the official installation guide from [bazel.build](https://docs.bazel.build/versions/4.1.0/install-windows.html).


## Clone and configure the repository

Clone the Lluvia repository from Github:

```powershell
git clone https://github.com/jadarve/lluvia.git
cd lluvia
```

Open `platform/values.bzl` and change the paths to Python2 and Python3 according to your instalation. Initially the file looks like this:

```python
# Linux
python2_path_linux = "/usr/bin/python2"
python3_path_linux = "/usr/bin/python3"

# Windows
python2_path_windows = "C:/Python27/python.exe"

# get this value by running where.exe python3
python3_path_windows = "C:/hostedtoolcache/windows/Python/3.7.9/x64/python3.exe"
```

## Build the C++ libraries

```powershell
bazel build //lluvia/cpp/...
```
Run the tests to verify that your compilation runs properly:

```powershell
bazel test //lluvia/cpp/...
```


## Python3 package

To build the Python3 package, execute the commands below from the repository's top-level directory.

```powershell
bazel build //lluvia/python:lluvia_wheel
pip3 install bazel-bin/lluvia/python/lluvia-0.0.1-py3-none-any.whl
```

Open a Python3 interpreter and import lluvia package

```python 
import lluvia as ll

session = ll.createSession()
```

If the import completes successfully, lluvia is ready to use.
