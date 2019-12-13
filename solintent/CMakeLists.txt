set(sources
	CommandLineInterface.cpp
	CommandLineInterface.h
	main.cpp
)

add_executable(solintent ${sources})
target_link_libraries(solintent PRIVATE solidity Boost::boost Boost::program_options)
