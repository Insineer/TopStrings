cmake_minimum_required(VERSION 3.8)
project(TopStrings)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c test.h test.c vector.h vector.c top_strings.h top_strings.c thread_pool.h thread_pool.c list.h)
add_executable(TopStrings ${SOURCE_FILES})

target_link_libraries(TopStrings pthread m)
