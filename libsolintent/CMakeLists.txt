set(sources
    ir/ExpressionSummary.cpp
    ir/ExpressionSummary.h
    ir/ForwardExpressionSummary.h
    static/BoundChecker.cpp
    static/BoundChecker.h
    util/Generic.h
    util/SourceLocation.cpp
    util/SourceLocation.h
)

add_library(intent ${sources})
target_link_libraries(intent PUBLIC solidity)
