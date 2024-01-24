#!/bin/bash
cmake -B build -G "Unix Makefiles"
make -C build
./build/test_wick.exe