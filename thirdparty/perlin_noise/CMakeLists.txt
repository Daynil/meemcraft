cmake_minimum_required(VERSION 3.1)
project(perlin_noise)

add_library(perlin_noise)
target_sources(perlin_noise PRIVATE "src/perlin_noise")
target_include_directories(perlin_noise PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")



