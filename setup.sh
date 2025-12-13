#!/bin/bash

sudo apt-get update
sudo apt-get install -y build-essential gcc autoconf libtool pkg-config make cmake cmake-gui cmake-curses-gui git ninja-build

if [ "$1" = "setup-broker" ]; then
  sudo apt-add-repository -y ppa:mosquitto-dev/mosquitto-ppa
  sudo apt-get update
  sudo apt-get install -y libc-ares-dev libssl-dev mosquitto # install mosquitto broker directly

  # disable auto-start mosquitto service
  sudo systemctl disable mosquitto.service
  sudo systemctl stop mosquitto.service
  # hack cleanup if any instance is running
  sudo pkill -9 mosquitto
fi

TMP_FOLDER="/tmp/mosquitto"

rm -rf $TMP_FOLDER
mkdir -p $TMP_FOLDER
pushd $TMP_FOLDER
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
make -j$(nproc)
sudo make uninstall # clean up first
sudo make install   # install mosquitto c lib
popd

# git submodule update --init
git submodule update --init --recursive

# Build and Install gRPC
export MY_INSTALL_DIR=$HOME/.local
mkdir -p $MY_INSTALL_DIR
export PATH="$MY_INSTALL_DIR/bin:$PATH"

# cd libs/grpc
mkdir -p cmake/build
pushd cmake/build
cmake -G Ninja \
  -DBUILD_SHARED_LIBS=ON \
  -DgRPC_INSTALL=ON \
  -DgRPC_BUILD_TESTS=OFF -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF \
  -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
  -DCMAKE_C_COMPILER=gcc-13 -DCMAKE_CXX_COMPILER=g++-13 \
  ../..
cmake --build . --target install -- -j$(nproc)
popd
