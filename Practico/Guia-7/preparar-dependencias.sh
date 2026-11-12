#!/usr/bin/env bash
set -euo pipefail
# Ubuntu 24.04 (ejecutar una sola vez si faltan las dependencias).
sudo apt-get update
sudo apt-get install -y build-essential make libglm-dev libglfw3-dev libgl1-mesa-dev
