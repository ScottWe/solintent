cmake_minimum_required(VERSION 3.9.0)

# Description
set(PROJECT_VERSION "0.0.1")
project(solintent VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# Sets compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# Loads Boost
set(Boost_USE_MULTITHREADED ON)
option(Boost_USE_STATIC_LIBS "Link Boost statically" ON)

set(BOOST_COMPONENTS "filesystem;unit_test_framework;program_options;system")

find_package(Boost 1.65.0 QUIET REQUIRED COMPONENTS ${BOOST_COMPONENTS})

# Loads Z3
find_package(Z3 QUIET REQUIRED)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "[Ww]indows")
  foreach (z3_lib ${Z3_LIBRARIES})
    message(STATUS "Adding copy rule for ${z3_lib}")
    add_custom_command(TARGET cpp_example
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${z3_lib}>
        $<TARGET_FILE_DIR:cpp_example>
    )
  endforeach()
endif()

# Subprojects
include_directories(solidity)
add_subdirectory(solidity)

# Project
include_directories(.)
add_subdirectory(solintent)
add_subdirectory(libsolintent)
add_subdirectory(test)
