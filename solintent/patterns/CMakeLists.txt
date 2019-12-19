set(sources
    DynamicArraysAsFixedContainers.cpp
    DynamicArraysAsFixedContainers.h
)

add_library(pattern ${sources})
target_link_libraries(pattern PUBLIC intent)
