#!/usr/bin/env bash

# This script installs lluvia Python package on a Google colab environment.
#
# Usage:
#
# Make sure the environment has a GPU enabled.
#
# In a cell, paste the following two statements
#
#    !git clone https://github.com/jadarve/lluvia.git
#    !./lluvia/scripts/install_google_colab.sh
#
# After that, import lluvia in a cell as:
#
#    import lluvia as ll


# Vulkan SDK
wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | apt-key add -
wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.2.176-bionic.list https://packages.lunarg.com/vulkan/1.2.176/lunarg-vulkan-1.2.176-bionic.list

# Bazel
apt install apt-transport-https curl gnupg
curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg
mv bazel.gpg /etc/apt/trusted.gpg.d/
echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list

apt update
apt install -y vulkan-sdk bazel

# Go to lluvia's directory
cd lluvia

# Build
bazel build --curses=no //lluvia/python:lluvia_wheel

# Install
pip3 install bazel-bin/lluvia/python/lluvia-0.0.1-py3-none-any.whl
