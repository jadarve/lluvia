FROM ubuntu:18.04

MAINTAINER Juan David Adarve Bermudez <juanda0718@gmail.com>

ENV LUNARG_VERSION="1.1.82"

RUN apt-get update \
    && apt-get install -y \
        software-properties-common \
        ca-certificates \
        wget \
        build-essential \
        cmake \
        clang \
        python3-pip \
        python-pip \
        unzip \
    && rm -rf /var/lib/apt/lists/*


# VULKAN SDK
RUN wget -qO - http://packages.lunarg.com/lunarg-signing-key-pub.asc >> lunarg-signing-key-pub.asc
RUN apt-key add lunarg-signing-key-pub.asc
RUN wget -qO /etc/apt/sources.list.d/lunarg-vulkan-$LUNARG_VERSION-bionic.list http://packages.lunarg.com/vulkan/$LUNARG_VERSION/lunarg-vulkan-$LUNARG_VERSION-bionic.list
RUN apt-get update \
    && apt-get install -y \
        lunarg-vulkan-sdk \
    && rm -rf /var/lib/apt/lists/*

# Python
RUN pip2 install --no-cache-dir cython numpy pytest
RUN pip3 install --no-cache-dir cython numpy pytest

COPY ./build.sh /home
RUN chmod +x /home/build.sh

ENTRYPOINT ["/home/build.sh"]
