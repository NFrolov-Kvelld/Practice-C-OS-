cmake_minimum_required(VERSION 3.10)
project(StaticLibraryExample)

set(CMAKE_C_STANDARD 11)

add_library(asgard_lib STATIC
    src/baldur.c
    src/loki.c
    src/tor.c
)

target_include_directories(asgard_lib PUBLIC include)

add_executable(main src/main.c)

target_link_libraries(main PRIVATE asgard_lib)