# With Ubuntu 22.04, there is no need to install Mesa drivers
# from third-party ppa. The default driver already has lavapipe
# and vulkaninfo from the LunarG SDK works correctly.
FROM ubuntu:22.04

LABEL Author="Juan David Adarve Bermudez <juanda0718@gmail.com>"

ARG DEBIAN_FRONTEND=noninteractive
ARG LUNARG_VERSION="1.3.211"
ARG HUGO_VERSION="0.98.0"

ENV TZ=Etc/UTC

# Base dependencies
RUN apt update \
    && apt install -y \
        python3-dev \
        software-properties-common \
        ca-certificates \
        curl \
        wget \
        build-essential \
        clang \
        python3-pip \
        git \
        unzip \
        golang \
        doxygen \
        graphviz \
        nodejs \
        npm \
    && rm -rf /var/lib/apt/lists/*

RUN wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | apt-key add - \
    && wget -qO /etc/apt/sources.list.d/lunarg-vulkan-$LUNARG_VERSION-focal.list https://packages.lunarg.com/vulkan/$LUNARG_VERSION/lunarg-vulkan-$LUNARG_VERSION-focal.list \
    && apt update \
    && apt upgrade -y \
    && apt install -y vulkan-sdk mesa-vulkan-drivers \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir \
    numpy \
    pytest \
    jinja2 \
    markupsafe \
    imageio \
    sphinx \
    sphinx-rtd-theme

RUN wget https://github.com/gohugoio/hugo/releases/download/v${HUGO_VERSION}/hugo_extended_${HUGO_VERSION}_Linux-64bit.deb \
    && dpkg -i hugo_extended_${HUGO_VERSION}_Linux-64bit.deb \
    && rm hugo_extended_${HUGO_VERSION}_Linux-64bit.deb

# Bazel
RUN curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor >bazel-archive-keyring.gpg \
    && mv bazel-archive-keyring.gpg /usr/share/keyrings \
    && echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list \
    && apt update \
    && apt install bazel \
    && rm -rf /var/lib/apt/lists/*
