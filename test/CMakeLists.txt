set(sources
    CompilerFramework.cpp
    CompilerFramework.h
)

add_executable(testsuite ${sources} main.cpp)
target_link_libraries(testsuite PRIVATE libsolc yul solidity yulInterpreter evmasm devcore Boost::boost Boost::program_options Boost::unit_test_framework evmc)
