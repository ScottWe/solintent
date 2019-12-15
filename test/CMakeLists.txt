set(sources
    CompilerFramework.cpp
    CompilerFramework.h
    libsolintent/ir/ExpressionSummaryTest.cpp
    libsolintent/static/BoundCheckerTest.cpp
    libsolintent/util/GenericTest.cpp
    libsolintent/util/SourceLocationTest.cpp
)

add_executable(testsuite ${sources} main.cpp)
target_link_libraries(testsuite PRIVATE intent libsolc yul solidity yulInterpreter evmasm devcore Boost::boost Boost::program_options Boost::unit_test_framework evmc)
