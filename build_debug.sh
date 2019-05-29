#!/bin/bash

SRC_PATH="./src";
DEBUG_PATH="./bin/debug";

mkdir -p $DEBUG_PATH

g++ -DDEBUG -std=c++11 -Wall -Werror -I $SRC_PATH $SRC_PATH/main.cpp $SRC_PATH/table/table.cpp $SRC_PATH/cell/cell.cpp -o $DEBUG_PATH/ixcell