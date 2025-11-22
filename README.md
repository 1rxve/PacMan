# PacMan Game

[![CircleCI](https://dl.circleci.com/status-badge/img/gh/1rxve/PacMan/tree/master.svg?style=svg&circle-token=CCIPRJ_2pdXksoTtJa9CnoeDtsAJL_588665523105f27daed04471a57105177d4d9ae0)](https://dl.circleci.com/status-badge/redirect/gh/1rxve/PacMan/tree/master)
Advanced Programming Project 2025-2026  
**Student:** Odia | **Student Number:** [s0244663]

## 📋 About
C++ implementation of PacMan using SFML. Demonstrates software architecture, design patterns (MVC, Observer, Factory, Singleton, State), and modern C++ practices.

## 🔨 Build & Run
```bash
mkdir build && cd build
cmake ..
make PacManGame
./representation/PacManGame
```

## 📁 Structure
```
├── logic/                      # Game logic library (no SFML)
│   ├── include/
│   └── src/
├── representation/             # SFML visualization
│   ├── include/
│   └── src/
│       └── main.cpp
├── resources/                  # Assets
└── .circleci/
    └── config.yml
```

## ✅ CI/CD
- Automatic builds on every commit
- Logic library compiles independently (no SFML)
- Full game tested with SFML integration

## 🛠️ Requirements
- CMake 3.16+
- C++17 compiler
- SFML 2.5+

---
*University of Antwerp - Advanced Programming 2025-2026*