#!/bin/bash
shopt -s globstar
clang-format include/*.hpp source/*.cpp -i -style=file