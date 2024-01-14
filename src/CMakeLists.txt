project (summarizaion_app)
cmake_minimum_required(VERSION 3.13)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories("${PROJECT_INCLUDE_DIR}")

link_directories(${LIBRARY_OUTPUT_PATH})

set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/)

set(PROJ_SRCS ${PROJECT_SOURCE_DIR}/main.cpp)
add_executable(${PROJECT_NAME} ${PROJ_SRCS})
target_link_libraries(${PROJECT_NAME} word2vec ${LIBS})
