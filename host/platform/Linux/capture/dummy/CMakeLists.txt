cmake_minimum_required(VERSION 3.10)
project(capture_dummy LANGUAGES C)

add_library(capture_dummy STATIC
  src/dummy.c
)

target_link_libraries(capture_dummy
  lg_common
)
