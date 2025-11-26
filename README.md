# PacMan Game

[![CircleCI](https://dl.circleci.com/status-badge/img/gh/1rxve/PacMan/tree/master.svg?style=svg&circle-token=CCIPRJ_2pdXksoTtJa9CnoeDtsAJL_588665523105f27daed04471a57105177d4d9ae0)](https://dl.circleci.com/status-badge/redirect/gh/1rxve/PacMan/tree/master)

Advanced Programming Project 2025-2026  
**Student:** Odia | **Student Number:** s0244663

## 📋 About
C++ implementation of PacMan using SFML. Demonstrates clean software architecture with separation between game logic and representation, implementing five key design patterns: MVC, Observer, Abstract Factory, Singleton, and State Pattern.

## 🔨 Build & Run

### Prerequisites
- CMake 3.28+
- C++17 compiler (GCC 13+ / Clang 18+)
- SFML 2.6+

### Building
```bash
mkdir build && cd build
cmake ..
make
./representation/PacManGame
```

## 📁 Project Structure
```
PacManGame/
├── logic/                          # Game logic library (SFML-free)
│   ├── CMakeLists.txt
│   ├── include/logic/
│   │   ├── entities/               # Game entities (PacMan, Ghost, etc.)
│   │   ├── world/                  # World management
│   │   ├── patterns/               # Design pattern interfaces
│   │   └── utils/                  # Utilities (Stopwatch, Random, Score)
│   └── src/
│       ├── entities/
│       ├── world/
│       ├── patterns/
│       └── utils/
│
├── representation/                 # SFML visualization
│   ├── CMakeLists.txt
│   ├── include/representation/
│   │   ├── views/                  # Entity views
│   │   ├── states/                 # Game states (Menu, Level, Pause)
│   │   ├── Camera.h                # Coordinate conversion
│   │   ├── Game.h                  # Main game loop
│   │   └── ConcreteFactory.h       # Entity factory implementation
│   └── src/
│       ├── views/
│       ├── states/
│       └── main.cpp                # Entry point
│
├── resources/                      # Game assets
│   └── sprites/
│       └── pacman_sprites.png
│
├── .circleci/
│   └── config.yml                  # CI/CD configuration
├── .clang-format                   # Code formatting rules
├── .gitignore
└── README.md
```

## 🏗️ Architecture

### Normalized Coordinate System
- Game logic uses normalized coordinates [-1, 1] for resolution independence
- Camera class converts normalized → pixel coordinates for rendering
- Logic library compiles without SFML dependencies

### Design Patterns
- **MVC**: Separation between Model (logic), View (representation), Controller (World/Game)
- **Observer**: Score updates and view synchronization
- **Abstract Factory**: Entity creation without coupling to SFML
- **Singleton**: Stopwatch and Random utilities
- **State Pattern**: Menu, gameplay, pause, and victory states

### Memory Management
- Smart pointers throughout (unique_ptr, shared_ptr, weak_ptr)
- No raw pointers except where design patterns require it
- Valgrind-verified memory leak free

## ✅ CI/CD
- Automatic builds on every commit via CircleCI
- Logic library tested independently (no SFML required)
- Cross-platform compatibility (Ubuntu 24.04)

## 🎮 Gameplay Features
- Classic PacMan maze navigation
- Four ghosts with different AI behaviors
- Coin collection and fruit power-ups
- Fear mode when eating fruits
- Multiple lives and level progression
- High score tracking with persistent storage

## 🛠️ Development Tools
- **Build System**: CMake
- **CI/CD**: CircleCI
- **Code Formatting**: clang-format
- **Memory Checking**: Valgrind
- **Version Control**: Git

---
*University of Antwerp - Advanced Programming 2025-2026*