# Minimum cmake version was deduced by
# the version of cmake shipped along with debian buster
cmake_minimum_required(VERSION 3.13)

project(summurization CXX)
set(output summurization_app)
# setup module path for include() & find_package() stuff
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-utils)
if (EnableVerboseLogging)
  message(STATUS "CMake module path: ${CMAKE_MODULE_PATH}")
endif ()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(
    STATUS "Setting build type to 'RelWithDebInfo' as none was specified")
  set(CMAKE_BUILD_TYPE
    RelWithDebInfo
    CACHE STRING "Choose the type of build" FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

# Setting default output directory for targets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
if (EnableVerboseLogging)
  message(STATUS "CMake runtime output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif ()

# Link against this library to set C++ standards
add_library(project_options INTERFACE)
include(CxxStandardSettings)
set_cxx_standard(project_options)

# Link against this library to use standard warnings
add_library(project_warnings INTERFACE)
include(CompilerWarnings)
set_project_warnings(project_warnings)

# Test projects, link against this library.
# These warnings are optimised to work with
# googletest/googlemock framework
add_library(test_project_warnings INTERFACE)
set_test_project_warnings(test_project_warnings)

# Collate target source files used for
# - clangformat
# - cpplint
# - doxygen
set(EXCLUDE_REGEX "${CMAKE_SOURCE_DIR}/build\|cmake-.*\/")
file(GLOB_RECURSE TARGET_SOURCE_FILES
  *.cpp
  *.cc
  *.c
  *.hpp
  *.hh
  *.h)
foreach(SRC ${TARGET_SOURCE_FILES})
  if (SRC MATCHES "${EXCLUDE_REGEX}")
    list(REMOVE_ITEM TARGET_SOURCE_FILES ${SRC})
  endif ()
endforeach ()

if (EnableVerboseLogging)
  message(STATUS "Target source list: ${TARGET_SOURCE_FILES}")
endif ()

# Bringing in code analysis, code formatting and documentation
# facilities etc.



# Add global project include directory
set(src_dir ${CMAKE_SOURCE_DIR}/src)
include_directories(${src_dir})

# set(APP_SOURCES )
# set(APP_HEADERS )

add_executable(${output} ${APP_SOURCES} ${APP_HEADERS})
