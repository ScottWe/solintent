set(sources
	CommandLineInterface.cpp
	CommandLineInterface.h
	main.cpp
)

add_subdirectory(asserts)
add_subdirectory(patterns)

add_executable(solintent ${sources})
target_link_libraries(solintent
					  PRIVATE intent pattern assert solidity Boost::boost
					          Boost::program_options)
