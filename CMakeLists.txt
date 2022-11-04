cmake_minimum_required(VERSION 3.5)
project(dbc-driver-gen
  VERSION 0.1.0
  LANGUAGES C CXX
)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

add_library(dbc-driver SHARED
  src/dbc-driver.cpp
)

target_include_directories(dbc-driver
  PUBLIC
    # Different paths for includes for build and install phase supported
    # via INSTALL_INTERFACE and BUILD_INTERFACE generator expressions.
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(dbc-driver PROPERTIES
  VERSION ${dbc-driver-gen_VERSION}
  SOVERSION ${dbc-driver-gen_VERSION_MAJOR}
  PUBLIC_HEADER "include/dbc-driver-gen/dbc-driver.hpp"
)

set(CMAKE_INSTALL_CMAKEDIR share/dbc-driver-gen/cmake)

install(DIRECTORY include/dbc-driver-gen/
  DESTINATION include/${PROJECT_NAME}
)

install(
  TARGETS dbc-driver
  EXPORT dbc-driver-genTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  PUBLIC_HEADER DESTINATION include
)

install(EXPORT dbc-driver-genTargets
  FILE dbc-driver-genTargets.cmake
  NAMESPACE DriverGen::
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(dbc-driver-genConfigVersion.cmake
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/dbc-driver-genConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

install(FILES dbc-driver-genConfig.cmake
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
)
