---
title: "Ubuntu 20.04"
date: 2021-06-09T00:00:00-05:00
weight: 1
---


## Dependencies

* Install the following packages in your system if they not available yet:

    ```bash
    apt install \
        build-essential \
        clang
    ```

* [Bazel](https://bazel.build/):

    ```bash
    curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
    echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
    apt update
    apt install bazel
    ```

* [LunarG Vulkan SDK](https://www.lunarg.com/vulkan-sdk/):

    ```bash
    wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add -
    sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.2.176-focal.list https://packages.lunarg.com/vulkan/1.2.176/lunarg-vulkan-1.2.176-focal.list
    sudo apt update
    sudo apt install -y vulkan-sdk
    ```

    Verify that the SDK was successfully installed by running:

    ```bash 
    vulkaninfo
    ```


## Build C++ Libraries

Clone and compile Lluvia's C++ libraries:

```bash
git clone https://github.com/jadarve/lluvia.git
cd lluvia

# install requirements listed in lluvia's root folder
sudo pip3 install -r requirements.txt

bazel build //lluvia/cpp/...
```

Run the tests to verify that your compilation runs properly:

```bash
bazel test //lluvia/cpp/...
```


## Python3 package

To build the Python3 package, execute the commands below from the repository's top-level directory. You can create a [virtual environment](https://virtualenv.pypa.io/en/latest/) to isolate the installation:

```bash
bazel build //lluvia/python:lluvia_wheel
pip3 install bazel-bin/lluvia/python/lluvia-0.0.1-py3-none-any.whl
```

Open a Python3 interpreter and import lluvia:

```python 
import lluvia as ll

session = ll.createSession()
```

If the import completes successfully, lluvia is ready to use.
