set(sources
    ir/ExpressionInterface.cpp
    ir/ExpressionInterface.h
    ir/ExpressionSummary.cpp
    ir/ExpressionSummary.h
    ir/ForwardExpressionSummary.h
    ir/IRVisitor.cpp
    ir/IRVisitor.h
    static/AbstractAnalyzer.cpp
    static/AbstractAnalyzer.h
    static/AnalysisEngine.cpp
    static/AnalysisEngine.h
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
