cmake_minimum_required(VERSION 3.7)

project(cg_2 LANGUAGES CXX)

add_compile_options(-Wall -Wextra -O3 -ffast-math -flto -march=native)

include_directories("lib")
file(GLOB_RECURSE SRC RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp" "lib/*.h")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)