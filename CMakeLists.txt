cmake_minimum_required(VERSION 3.18.0)

project(MAS_tests)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter -Wno-switch")

option(BUILD_SHARED_LIBS      "Build tests"    ON)
option(BUILD_TESTS      "Build tests"    OFF)
option(BUILD_EXAMPLES   "Build examples" OFF)
option(UTPP_INCLUDE_TESTS_IN_BUILD   "Build tests" OFF)

include(FetchContent)

FetchContent_Declare(UnitTest++
        GIT_REPOSITORY https://github.com/unittest-cpp/unittest-cpp.git
        GIT_TAG  tags/v2.0.0)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json/
        GIT_TAG  tags/v3.11.2)

FetchContent_Declare(json-schema-validator
        GIT_REPOSITORY https://github.com/pboettch/json-schema-validator.git
        GIT_TAG  origin/main)

FetchContent_MakeAvailable(UnitTest++ json json-schema-validator)

# Patch json-schema-validator to skip duplicate schema insertions instead of throwing
# This fixes an issue where schemas with shared $ref dependencies cause "already inserted" errors
file(READ "${CMAKE_BINARY_DIR}/_deps/json-schema-validator-src/src/json-validator.cpp" JSON_VALIDATOR_CPP)
string(REPLACE 
    "throw std::invalid_argument(\"schema with \" + uri.to_string() + \" already inserted\");"
    "return; // Skip if already inserted (patched by MAS)"
    JSON_VALIDATOR_CPP "${JSON_VALIDATOR_CPP}")
file(WRITE "${CMAKE_BINARY_DIR}/_deps/json-schema-validator-src/src/json-validator.cpp" "${JSON_VALIDATOR_CPP}")

SET(CMAKE_CXX_FLAGS "-Wall -Wextra")

include_directories("${CMAKE_BINARY_DIR}/_deps/unittest++-src/UnitTest++/")
include_directories("${CMAKE_BINARY_DIR}/_deps/json-src/include/nlohmann/")

file(GLOB MAS_sources
    "tests/*.cpp")

add_executable(MAS_tests ${MAS_sources})
target_link_libraries(MAS_tests nlohmann_json::nlohmann_json nlohmann_json_schema_validator UnitTest++)


