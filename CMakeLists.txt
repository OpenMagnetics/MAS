cmake_minimum_required(VERSION 3.18.0)

project(MAS_tests)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter -Wno-switch")

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

FetchContent_MakeAvailable(UnitTest++ json json-schema-validator)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra")

find_program(NAMES quicktype REQUIRED)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/MAS")

add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/MAS/CoreTemplate.cpp"
  COMMAND quicktype -s schema ../schemas/core.json -S ../schemas/utils.json -S ../schemas/core/gap.json -S ../schemas/core/shape.json -S ../schemas/core/material.json -S ../schemas/insulation/material.json -S ../schemas/insulation/wireCoating.json -S ../schemas/core/bobbin.json -o MAS/CoreTemplate.cpp --namespace OpenMagnetics --source-style multi-source --type-style pascal-case --member-style underscore-case --enumerator-style upper-underscore-case --no-boost
  USES_TERMINAL)
include_directories("${CMAKE_BINARY_DIR}/_deps/unittest++-src/UnitTest++/")
include_directories("${CMAKE_BINARY_DIR}/MAS/")

add_custom_target(run ALL
    DEPENDS "${CMAKE_BINARY_DIR}/MAS/CoreTemplate.cpp")

file(GLOB MAS_sources
    "tests/*.cpp")

add_executable(MAS_tests ${MAS_sources})
target_link_libraries(MAS_tests nlohmann_json::nlohmann_json nlohmann_json_schema_validator UnitTest++)
