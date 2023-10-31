cmake_minimum_required(VERSION 3.25)
project(Wick LANGUAGES CXX)
include_directories(include)
add_executable(Wick source/bird.cpp source/main.cpp)