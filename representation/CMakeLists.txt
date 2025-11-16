# ============================================
# REPRESENTATION (SFML + Logic Library)
# ============================================

# Zoek SFML (ALLEEN hier!)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Verzamel alle representation source files
set(REPRESENTATION_SOURCES
        src/main.cpp
        # src/Game.cpp
        # src/Camera.cpp
        # src/views/PacManView.cpp
        # ... (voeg later toe)
)

# Maak executable
add_executable(PacManGame ${REPRESENTATION_SOURCES})

# Link met Logic library + SFML
target_link_libraries(PacManGame PRIVATE
        GameLogic              # Onze logic library!
        sfml-graphics
        sfml-window
        sfml-system
)

# Include directories
target_include_directories(PacManGame PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Optioneel: warnings
target_compile_options(PacManGame PRIVATE
        -Wall -Wextra -Wpedantic
)