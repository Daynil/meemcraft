cmake_minimum_required(VERSION 3.1)
project(thread_pool)

add_library(thread_pool)
target_sources(thread_pool PRIVATE "src/thread_pool")
target_include_directories(thread_pool PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")



