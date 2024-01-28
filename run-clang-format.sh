#!/bin/bash
shopt -s globstar
clang-format include/*.hpp source/*.cpp test/*.cpp -i -style=file