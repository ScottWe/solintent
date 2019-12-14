set(sources
    static/BoundChecker.cpp
    static/BoundChecker.h
    util/SourceLocation.cpp
    util/SourceLocation.h
)

add_library(intent ${sources})
target_link_libraries(intent PUBLIC solidity)
