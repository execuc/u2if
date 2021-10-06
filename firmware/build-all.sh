#!/bin/bash

dirname=${PWD##*/} 
if [ $dirname != "firmware" ]; then
  echo "This script has to be launched in the firmware directory"
  exit -1
fi

VERSION=$(cat $PWD/source/CMakeLists.txt | grep 'project(u2if VERSION' | awk -F'[=\"]' '{print $2}')
RELEASE_DIR=$PWD"/release"
CORES_NB=$(nproc --all)

function build {
    echo "Build for board $2 (I2S_ALLOW=$3, HUB75_ALLOW=$6, WS2812=$4 with $5 max leds)"
    tmp_dir=$(mktemp -d -t ci-XXXXXXXXXX)
    FIRMWARE_ROOT_DIR=$PWD
    cd $tmp_dir
    cmake -DBOARD=$2 -DI2S_ALLOW=$3 -DWS2812_ENABLED=$4 -DWS2812_SIZE=$5 -DHUB75_ALLOW=$6 $FIRMWARE_ROOT_DIR/source
    make -j$CORES_NB
    cp u2if.uf2 $RELEASE_DIR/u2if_$1_v$VERSION.uf2
    cd $FIRMWARE_ROOT_DIR
    rm -rf $tmp_dir
}


mkdir -p $RELEASE_DIR
build pico_i2s PICO 1 1 1000 0
build pico_hub75 PICO 0 1 1000 1
build feather FEATHER 0 1 1000 0
build itsybitsy ITSYBITSY 0 1 1000 0
build qtpy QTPY 0 1 1000 0
build qt2040_trinkey QT2040_TRINKEY 0 0 1000 0
build macropad MACROPAD 0 1 12 0

