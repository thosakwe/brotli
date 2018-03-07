cmake_minimum_required(VERSION 3.6.0)

project(dart_brotli)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
set(CMAKE_CXX_STANDARD 11)

if(NOT $ENV{DART_SDK} STREQUAL "")
    message("Dart SDK path: $ENV{DART_SDK}")
else()
    message(FATAL_ERROR "DART_SDK is not defined")
endif()


ExternalProject_Add(brotli
  GIT_REPOSITORY "https://github.com/google/brotli.git"
  GIT_SHALLOW 1
)

ExternalProject_Get_Property(brotli install_dir)
include_directories(${install_dir}/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -undefined dynamic_lookup")

include_directories($ENV{DART_SDK}/include)
add_definitions(-DDART_SHARED_LIB=1)
add_library(dart_brotli SHARED lib/brotli.cc)
target_link_libraries(dart_brotli brotlienc brotlidec)

install(TARGETS dart_brotli DESTINATION lib)