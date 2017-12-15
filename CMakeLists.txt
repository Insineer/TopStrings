cmake_minimum_required(VERSION 3.8)
project(TopStrings)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
        src/main.c
        src/test.h
        src/test.c
        src/top_strings.h
        src/top_strings.c
        src/useful/list.h
        src/useful/list.c
        src/useful/vector.h
        src/useful/vector.c )

add_executable(TopStrings ${SOURCE_FILES})

target_link_libraries(TopStrings pthread m)
