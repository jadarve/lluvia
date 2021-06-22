#!/usr/bin/env bash

# Vulkan SDK
wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | apt-key add -
wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.2.176-bionic.list https://packages.lunarg.com/vulkan/1.2.176/lunarg-vulkan-1.2.176-bionic.list
apt update
apt install -y vulkan-sdk

# Bazel
apt install apt-transport-https curl gnupg
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg
mv bazel.gpg /etc/apt/trusted.gpg.d/
echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
apt update && apt install -y bazel

# Build
bazel build //lluvia/python:lluvia_wheel

# Install
pip3 install lluvia/bazel-bin/lluvia/python/lluvia-0.0.1-py3-none-any.whl
