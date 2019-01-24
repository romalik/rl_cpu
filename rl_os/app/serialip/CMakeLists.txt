cmake_minimum_required(VERSION 2.8)

project(uip_slip_userspace C)


include_directories(./)

file(GLOB_RECURSE UIP_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "uip/*.c")
add_library(uip_slip ${UIP_SOURCES})

add_executable(uip_test main.c)
target_link_libraries(uip_test uip_slip)

