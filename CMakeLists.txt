cmake_minimum_required(VERSION 3.25)
project(wick LANGUAGES CXX)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(include)
set(FILES
    environment.cpp
    errorReporter.cpp
    expression.cpp
    interpreter.cpp
    native.cpp
    parser.cpp
    persistentMap.cpp
    scanner.cpp
    statement.cpp
    token.cpp
    # Add other source files here.
)

list(TRANSFORM FILES PREPEND "source/" OUTPUT_VARIABLE SOURCE)

set(TEST_FILES
    scannerTest.cpp
    tokenTest.cpp)
    
list(TRANSFORM TEST_FILES PREPEND "test/" OUTPUT_VARIABLE TEST)

enable_testing()

add_executable(wick
    source/main.cpp
    ${SOURCE}
)

add_executable(test_wick
    test/main.cpp
    ${TEST}
    ${SOURCE}
)

add_test(NAME "Wick Tests" COMMAND test_wick)