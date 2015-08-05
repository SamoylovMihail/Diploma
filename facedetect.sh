#!/bin/bash


function build {
    build_dir=$1_build
    mkdir -p $build_dir
    cd $build_dir
    cmake ../$1
    make
    cd ..
}

build facecut
build facedetect
