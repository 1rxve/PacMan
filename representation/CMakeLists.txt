# ============================================
# REPRESENTATION (SFML + Logic Library)
# ============================================

# SFML voor Windows - ALLEEN het pad instellen
if(WIN32)
    set(SFML_DIR "C:/SFML-2.6.1/lib/cmake/SFML")
endif()

# Zoek SFML
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

# Kopieer DLL's NA het maken van PacManGame target
if(WIN32)
    add_custom_command(TARGET PacManGame POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "C:/SFML-2.6.1/bin"
            $<TARGET_FILE_DIR:PacManGame>
    )
endif()