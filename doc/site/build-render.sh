#!/usr/bin/env bash

npm install

git submodule update --init --recursive --depth 1
hugo --gc --minify


wget -O public/v0.0.5.zip https://github.com/jadarve/rules_vulkan/archive/refs/tags/v0.0.5.zip
unzip public/v0.0.5.zip -d public/rules_vulkan
