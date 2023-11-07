cmake_minimum_required(VERSION 3.25)
project(Wick LANGUAGES CXX)

include_directories(include)
set(FILES
    factorial.cpp
    # Add other source files here.
)

list(TRANSFORM FILES PREPEND "source/" OUTPUT_VARIABLE SOURCE)
list(TRANSFORM FILES PREPEND "test/" OUTPUT_VARIABLE TEST)

enable_testing()

add_executable(Wick
    source/main.cpp
    ${SOURCE}
)

add_executable(WickTest 
    ${TEST}
    ${SOURCE}
)

