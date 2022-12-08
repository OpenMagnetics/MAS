cmake_minimum_required(VERSION 3.18.0)

project(MAS_tests)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter -Wno-switch")

option(BUILD_TESTS      "Build tests"    OFF)
option(BUILD_EXAMPLES   "Build examples" OFF)

include(FetchContent)

FetchContent_Declare(UnitTest++
        GIT_REPOSITORY https://github.com/unittest-cpp/unittest-cpp.git
        GIT_TAG  tags/v2.0.0)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json/
        GIT_TAG  tags/v3.10.0)

FetchContent_Declare(json-schema-validator
        GIT_REPOSITORY https://github.com/pboettch/json-schema-validator.git
        GIT_TAG  tags/2.1.0)

FetchContent_Declare(magic-enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum
        GIT_TAG  tags/v0.8.1)

FetchContent_MakeAvailable(UnitTest++ json json-schema-validator magic-enum)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra")

include_directories("${CMAKE_BINARY_DIR}/_deps/unittest++-src/UnitTest++/")
include_directories("${CMAKE_BINARY_DIR}/_deps/json-src/include/nlohmann/")
include_directories("${CMAKE_BINARY_DIR}/_deps/magic-enum-src/include")

file(GLOB MAS_sources
    "tests/*.cpp")

add_executable(MAS_tests ${MAS_sources})
target_link_libraries(MAS_tests nlohmann_json::nlohmann_json nlohmann_json_schema_validator UnitTest++)


