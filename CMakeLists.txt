cmake_minimum_required(VERSION 3.8)
project(dbc-driver-gen
  VERSION 0.1.0
  LANGUAGES C CXX
)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# LIBRARY

add_library(${PROJECT_NAME} SHARED
  src/${PROJECT_NAME}.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC 
    # Different paths for includes for build and install phase supported
    # via INSTALL_INTERFACE and BUILD_INTERFACE generator expressions.
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${${PROJECT_NAME}_VERSION}
  SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR}
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

install(FILES include/${PROJECT_NAME}/${PROJECT_NAME}.hpp
  DESTINATION include/${PROJECT_NAME}
)

# END LIBRARY

# DBC-DRIVER

add_executable(dbc-driver
  src/dbc-driver.cpp
)

target_include_directories(dbc-driver
  PRIVATE 
    # Different paths for includes for build and install phase supported
    # via INSTALL_INTERFACE and BUILD_INTERFACE generator expressions.
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(dbc-driver ${PROJECT_NAME})

target_compile_features(dbc-driver PUBLIC cxx_std_17)
set_target_properties(dbc-driver PROPERTIES CXX_EXTENSIONS OFF)

# END DBC-DRIVER

# DBC-ROS2-DRIVER

add_executable(dbc-ros2-driver
  src/dbc-ros2-driver.cpp
)

target_include_directories(dbc-ros2-driver
  PRIVATE 
    # Different paths for includes for build and install phase supported
    # via INSTALL_INTERFACE and BUILD_INTERFACE generator expressions.
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(dbc-ros2-driver ${PROJECT_NAME})

target_compile_features(dbc-ros2-driver PUBLIC cxx_std_17)
set_target_properties(dbc-ros2-driver PROPERTIES CXX_EXTENSIONS OFF)

# END DBC-ROS2-DRIVER

# INSTALLATION

set(CMAKE_INSTALL_CMAKEDIR share/${PROJECT_NAME}/cmake)

install(
  TARGETS ${PROJECT_NAME} dbc-driver dbc-ros2-driver
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  PUBLIC_HEADER DESTINATION include
)

install(EXPORT dbc-driver-genTargets
  FILE dbc-driver-genTargets.cmake
  NAMESPACE DbcDriverGen::
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

install(DIRECTORY templates/
  DESTINATION share/${PROJECT_NAME}/templates
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

install(FILES ${PROJECT_NAME}Config.cmake
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)

# END INSTALLATION

# UNINSTALL HELPERS

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
)

# END UNINSTALL HELPERS
