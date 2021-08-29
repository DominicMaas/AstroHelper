#!/bin/bash

# Set correct location
mkdir build
cd build

# Ensure latest code (assuming cloned via HTTP)
git fetch && git pull 

# Ready to build!
cmake .. 
cmake --build .