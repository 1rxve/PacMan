# Optioneel maar aanbevolen
cmake_minimum_required(VERSION 3.28)
project(PacManGame-Representation)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
if(WIN32)
    set(SFML_DIR "C:/SFML-2.6.1/lib/cmake/SFML")
endif()
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Source files
set(REPRESENTATION_SOURCES
        src/main.cpp
        src/Camera.cpp
        src/views/PacManView.cpp
        src/views/EntityView.cpp
        src/views/WallView.cpp
        src/ConcreteFactory.cpp
        include/representation/ConcreteFactory.h
        src/views/CoinView.cpp
        include/representation/states/State.h
        src/StateManager.cpp
        include/representation/StateManager.h
        src/states/LevelState.cpp
        include/representation/states/LevelState.h
        src/Game.cpp
        include/representation/Game.h
        src/states/MenuState.cpp
        include/representation/states/MenuState.h
        src/states/PausedState.cpp
        include/representation/states/PausedState.h
        src/states/VictoryState.cpp
        include/representation/states/VictoryState.h
        src/states/NameEntryState.cpp
        include/representation/states/NameEntryState.h
        src/views/GhostView.cpp
        include/representation/views/GhostView.h
        src/views/DoorView.cpp
        include/representation/views/DoorView.h
        src/states/FruitView.cpp
        include/representation/views/FruitView.h
)

# Create executable
add_executable(PacManGame ${REPRESENTATION_SOURCES})

# Link libraries
target_link_libraries(PacManGame PRIVATE
        GameLogic
        sfml-graphics
        sfml-window
        sfml-system
)

# Include directories
target_include_directories(PacManGame PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Warnings
target_compile_options(PacManGame PRIVATE
        -Wall -Wextra -Wpedantic
)

# Copy SFML DLLs (Windows)
if(WIN32)
    add_custom_command(TARGET PacManGame POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "C:/SFML-2.6.1/bin"
            $<TARGET_FILE_DIR:PacManGame>
    )
endif()