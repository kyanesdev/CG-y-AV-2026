#!/usr/bin/env bash
set -euo pipefail
# Ubuntu 24.04; instala lo que usa el proyecto (OpenGL/GLFW/GLM/STB).
sudo apt-get update
sudo apt-get install -y build-essential make libglm-dev libglfw3-dev libgl1-mesa-dev libstb-dev
# La guia usa #include <stb_image.h> para que el include sea independiente del sistema.
mkdir -p third_party/stb
cp /usr/include/stb/stb_image.h third_party/stb/stb_image.h
printf 'Dependencias instaladas; listo para make y make run.\n'
