Installation (Ubuntu 18.04)
===========================

Dependencies
^^^^^^^^^^^^

Install the following packages in your system if they are still not available:

.. code-block:: bash

    apt install \
        build-essential \
        cmake \
        clang

Install Bazel:

.. code-block:: bash

    curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
    echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
    apt update
    apt install bazel

Install the `LunarG Vulkan SDK <https://www.lunarg.com/vulkan-sdk/>`_:

.. code-block:: bash

    wget -qO - http://packages.lunarg.com/lunarg-signing-key-pub.asc >> lunarg-signing-key-pub.asc
    apt-key add lunarg-signing-key-pub.asc
    wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.1.121-bionic.list http://packages.lunarg.com/vulkan/1.1.121/lunarg-vulkan-1.1.121-bionic.list
    apt-get update 
    apt-get install lunarg-vulkan-sdk

Verify that the SDK was successfully installed by running:

.. code-block:: bash
    
    vulkaninfo

C++ Libraries
^^^^^^^^^^^^^

Clone and compile Lluvia's C++ libraries:

.. code-block:: bash

    git clone https://github.com/jadarve/lluvia.git
    cd lluvia
    CC=clang bazel build //...

Run the tests to verify that your compilation runs properly:

.. code-block:: bash
    
    CC=clang bazel test //...


Python3 package
^^^^^^^^^^^^^^^

To build the Python3 package, execute the commands below from the repository's top-level directory. You can create a `virtual environment <https://virtualenv.pypa.io/en/latest/>`_ to isolate the installation:

.. code-block:: bash

    cd python
    pip3 install -r requirements.txt
    CC=clang python3 setup.py build
    python3 setup.py install


Open a Python3 interpreter and import lluvia package

.. code-block:: python
    
    import lluvia as ll
