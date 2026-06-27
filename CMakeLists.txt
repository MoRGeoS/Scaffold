cmake_minimum_required(VERSION 3.19)
project(Scaffold)

add_subdirectory(thirdparty/CLI11)
add_subdirectory(thirdparty/json)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)

add_executable(Scaffold
  ${SOURCE_FILES}
)

target_include_directories(Scaffold
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(Scaffold
  PRIVATE
    CLI11::CLI11
    nlohmann_json::nlohmann_json
)

set_target_properties(Scaffold
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)