cmake_minimum_required(VERSION 3.0)

project(sudo)

add_executable(sudo sudo.cpp)
add_executable(pipedcmd pipedcmd.cpp)

install(TARGETS sudo pipedcmd DESTINATION ./bin)