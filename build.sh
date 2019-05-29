#!/bin/bash

SRC_PATH="./src";
RELEASE_PATH="./bin/release";

mkdir -p $RELEASE_PATH

g++ -std=c++11 -Wall -Werror -I $SRC_PATH $SRC_PATH/main.cpp $SRC_PATH/table/table.cpp $SRC_PATH/cell/cell.cpp -o $RELEASE_PATH/ixcell