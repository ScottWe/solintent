set(sources
    CompilerFramework.cpp
    CompilerFramework.h
    libsolintent/ir/ExpressionSummaryTest.cpp
    libsolintent/ir/StatementSummaryTest.cpp
    libsolintent/ir/VisitorTest.cpp
    libsolintent/static/AnalysisEngineTest.cpp
    libsolintent/static/BoundCheckerTest.cpp
    libsolintent/static/CondCheckerTest.cpp
    libsolintent/static/ObligationTests.cpp
    libsolintent/static/StatementCheckerTests.cpp
    libsolintent/util/GenericTest.cpp
    libsolintent/util/SourceLocationTest.cpp
    solintent/GasConstraintOnLoopsTest.cpp
)

add_executable(testsuite ${sources} main.cpp)
target_link_libraries(testsuite
                      PRIVATE intent assert libsolc yul solidity yulInterpreter
                              evmasm devcore Boost::boost Boost::program_options
                              Boost::unit_test_framework evmc
)
