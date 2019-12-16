set(sources
    ir/ExpressionInterface.cpp
    ir/ExpressionInterface.h
    ir/ExpressionSummary.cpp
    ir/ExpressionSummary.h
    ir/ForwardExpressionSummary.h
    static/AbstractAnalyzer.h
    static/AbstractAnalyzer.cpp
    static/BoundChecker.cpp
    static/BoundChecker.h
    static/CondChecker.cpp
    static/CondChecker.h
    util/Generic.h
    util/SourceLocation.cpp
    util/SourceLocation.h
)

add_library(intent ${sources})
target_link_libraries(intent PUBLIC solidity)
