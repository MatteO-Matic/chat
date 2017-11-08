cmake_minimum_required(VERSION 2.8.9)
project(chat)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_STANDARD 14)

# Some other library that we import that was also built using CMake
# and has an exported target.
#find_package(MyOtherLibrary REQUIRED)

# Targets that we develop here
#enable_testing()
#add_subdirectory(liblib)
add_subdirectory(server)
