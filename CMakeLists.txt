cmake_minimum_required(VERSION 3.10)

project(summurization CXX)
set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})

set(CMAKE_BINARY_DIR ${PROJECT_ROOT_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_ROOT_DIR}/lib)
 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic ") #-Werror -fPIC -DNOT_USE_UTILS_FILE
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native -funroll-loops -ftree-vectorize -DNDEBUG")

set(PROJECT_INCLUDE_DIR ${PROJECT_ROOT_DIR}/include)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    set(LIBS "-pthread -lstdc++")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
endif()


# Add global project include directory
set(src_dir ${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${src_dir})
# add_subdirectory(src)

# set(APP_SOURCES )
# set(APP_HEADERS )

