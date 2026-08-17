cmake_minimum_required(VERSION 3.10.0)
project(argwc-comp VERSION 0.1.0 LANGUAGES C CXX)
option(BUILD_TESTS "Build argwc-comp tests." OFF)

add_library(argwc-comp src/argwc-comp.cpp src/Parser.cpp src/Tokenizer.cpp)
target_include_directories(argwc-comp PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(argwc-comp PRIVATE cxx_std_20)


if(BUILD_TESTS)
    enable_testing()

    add_executable(argwc-comp-tests
        tests/test.cpp
    )

    target_link_libraries(argwc-comp-tests PRIVATE argwc-comp)

    add_test(NAME argwc-comp-tests COMMAND argwc-comp-tests)
endif()