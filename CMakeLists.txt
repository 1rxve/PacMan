cmake_minimum_required(VERSION 3.28)
project(PacManGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Voeg subdirectories toe
add_subdirectory(logic)
add_subdirectory(representation)