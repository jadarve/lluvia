---
title: "Linux using docker"
date: 2022-05-03
weight: 2
---

Install docker following the [official documentation](https://docs.docker.com/engine/install/ubuntu/) and the [post installation guide](https://docs.docker.com/engine/install/linux-postinstall/).

Clone lluvia and build the container

```bash
git clone https://github.com/jadarve/lluvia.git
cd lluvia

docker build ci/ --tag="lluvia:local"
```

Run the container, mounting lluvia's repository at `/lluvia`:

```bash
docker run \
    --mount type=bind,source="$(pwd)",target=/lluvia \
    -it --rm "lluvia:local" /bin/bash
```

Inside the container, build and test all of lluvia package:

```bash
cd lluvia
bazel build //...
bazel test --test_output=errors //...
```
