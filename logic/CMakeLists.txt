# ============================================
# GAME LOGIC LIBRARY (GEEN SFML!)
# ============================================

# Verzamel alle logic source files
set(LOGIC_SOURCES
        # src/entities/EntityModel.cpp
        # src/entities/PacMan.cpp
        # src/entities/Ghost.cpp
        # src/world/World.cpp
        # ... (voeg later toe als je ze maakt)
)

# Als er nog geen .cpp files zijn, maak een dummy library
if(NOT LOGIC_SOURCES)
    # Dummy source om library te kunnen maken zonder files
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp "// Dummy file for empty library\n")
    set(LOGIC_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)
endif()

# Maak static library
add_library(GameLogic STATIC ${LOGIC_SOURCES})

# Public include directory (zodat representation het kan gebruiken)
target_include_directories(GameLogic PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# BELANGRIJK: GEEN find_package(SFML) hier!
# GEEN SFML links!
# Deze library moet compileren zonder SFML!

# Optioneel: warnings
target_compile_options(GameLogic PRIVATE
        -Wall -Wextra -Wpedantic
)