cmake_minimum_required(VERSION 3.15)
project(pcglite
  VERSION 0.1.0
  LANGUAGES CXX)

include(GNUInstallDirs)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-config
)

set(config_destination share/cmake/${PROJECT_NAME})
install(EXPORT ${PROJECT_NAME}-config
  DESTINATION ${config_destination}
  NAMESPACE ${PROJECT_NAME}::
)

set(version_file ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${version_file}
  COMPATIBILITY AnyNewerVersion
)
install(FILES ${version_file}
  DESTINATION ${config_destination}
)

include(CTest)
if(BUILD_TESTING AND "${CMAKE_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
  add_subdirectory(test)
endif()
