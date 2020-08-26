FROM ubuntu:18.04

MAINTAINER Juan David Adarve Bermudez <juanda0718@gmail.com>

ENV LUNARG_VERSION="1.1.130"

# First batch of packages
RUN apt-get update \
    && apt-get install -y \
        software-properties-common \
        ca-certificates \
        curl \
        wget \
        build-essential \
        cmake \
        clang \
        python3-pip \
        python3-numpy \
        python-pip \
        unzip \
        git \
    && rm -rf /var/lib/apt/lists/*

# Python
RUN pip2 install --no-cache-dir cython numpy pytest jinja2 markupsafe
RUN pip3 install --no-cache-dir cython numpy pytest jinja2 markupsafe

# Vulkan SDK
RUN wget -qO - http://packages.lunarg.com/lunarg-signing-key-pub.asc >> lunarg-signing-key-pub.asc
RUN apt-key add lunarg-signing-key-pub.asc
RUN wget -qO /etc/apt/sources.list.d/lunarg-vulkan-$LUNARG_VERSION-bionic.list http://packages.lunarg.com/vulkan/$LUNARG_VERSION/lunarg-vulkan-$LUNARG_VERSION-bionic.list

# Bazel
RUN curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
RUN echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list

RUN apt-get update \
    && apt-get install -y \
        lunarg-vulkan-sdk \
        bazel \
    && rm -rf /var/lib/apt/lists/*
