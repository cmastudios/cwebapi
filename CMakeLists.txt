cmake_minimum_required(VERSION 3.10)
project(webapitest)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_subdirectory(vendor)
add_subdirectory(api)
add_subdirectory(client)
