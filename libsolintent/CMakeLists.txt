set(sources
    ir/ExpressionInterface.cpp
    ir/ExpressionInterface.h
    ir/ExpressionSummary.cpp
    ir/ExpressionSummary.h
    ir/ForwardIR.h
    ir/IRSummary.cpp
    ir/IRSummary.h
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
    static/ImplicitObligation.cpp
    static/ImplicitObligation.h
    util/Generic.h
    util/SourceLocation.cpp
    util/SourceLocation.h
)

add_library(intent ${sources})
target_link_libraries(intent PUBLIC solidity)
