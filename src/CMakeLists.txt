cmake_minimum_required(VERSION 3.10)
project (summarizaion_app)
set(output summarizaion_app)
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include_directories("${PROJECT_INCLUDE_DIR}")

link_directories(${LIBRARY_OUTPUT_PATH})

set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/)

set(PROJ_SRCS 
  ${PROJECT_SOURCE_DIR}/main.cpp
  ${PROJECT_SOURCE_DIR}/tfidf.cpp
  # ${PROJECT_SOURCE_DIR}/pageRank.cpp
  # ${PROJECT_SOURCE_DIR}/utils.cpp
)
add_executable(${output} ${PROJ_SRCS})
target_link_libraries(${output} stdc++ -lpthread)
