set(sources
    GasConstraintOnLoops.cpp
    GasConstraintOnLoops.h
)

add_library(assert ${sources})
target_link_libraries(assert PUBLIC intent)
