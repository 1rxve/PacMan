# Optioneel maar aanbevolen
cmake_minimum_required(VERSION 3.28)
project(GameLogic)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(LOGIC_SOURCES
        src/entities/EntityModel.cpp
        src/entities/PacManModel.cpp
)

# Header files (IDE support)
set(LOGIC_HEADERS
        include/logic/entities/EntityModel.h
        include/logic/entities/PacManModel.h
)

# Create library
add_library(GameLogic STATIC ${LOGIC_SOURCES} ${LOGIC_HEADERS})

# Include directories
target_include_directories(GameLogic PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Warnings
target_compile_options(GameLogic PRIVATE
        -Wall -Wextra -Wpedantic
)