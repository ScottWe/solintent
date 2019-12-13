cmake_minimum_required(VERSION 3.9.0)

# Description
set(PROJECT_VERSION "0.0.1")
project(solintent VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# Sets compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Acquires Boost
set(Boost_USE_MULTITHREADED ON)
option(Boost_USE_STATIC_LIBS "Link Boost statically" ON)

set(BOOST_COMPONENTS "filesystem;unit_test_framework;program_options;system")

find_package(Boost 1.65.0 QUIET REQUIRED COMPONENTS ${BOOST_COMPONENTS})

# Subprojects
include_directories(solidity)
add_subdirectory(solidity)

# Project
