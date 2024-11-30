cmake_minimum_required(VERSION 3.16)

project(LRUCache C)

message("Hello")

add_library(lru_cache
    ${PROJECT_SOURCE_DIR}/cache.c
)

target_link_directories(lru_cache PUBLIC ".")

add_executable(problem_lc 
    ${PROJECT_SOURCE_DIR}/problem_lc.c
)

target_link_libraries(problem_lc lru_cache)