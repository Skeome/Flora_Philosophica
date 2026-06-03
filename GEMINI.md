# Flora Philosophia: Technical Context

## Project Overview
**Flora Philosophia** is a cross-platform (Web, Android, Desktop) RPG developed in C++. It serves as a stealth-educational tool for real-world plant alchemy (operative spagyrics). The game features a real-time astrological engine that synchronizes in-game quality and processes with the player's local real-world planetary hours.

### Key Technologies
- **Language:** C++17 (Standardized in `CMakeLists.txt`)
- **Rendering:** Raylib (2D, 3/4 top-down perspective)
- **Build System:** CMake (utilizing CPM for dependency management)
- **Platform Abstraction:** Emscripten (Web), Android NDK (Mobile), GLFW/X11/Wayland (Desktop)
- **Data Handling:** `nlohmann/json`
- **Testing:** `doctest`

## Building and Running

### Prerequisites
- **Raylib:** Must be installed system-wide (e.g., `libraylib-dev` on Debian/Ubuntu).
- **CMake:** Version 3.20 or higher.
- **Compiler:** C++17 compatible (GCC, Clang, or MSVC).

### Core Commands
- **Configure:** `cmake -B build`
- **Build:** `cmake --build build`
- **Run (Desktop):** `./scripts/run_game.sh` (This script handles Wayland/X11 compatibility).
- **Test:**
  - Build tests: `cmake --build build --target FloraPhilosophiaTests`
  - Run tests: `./build/FloraPhilosophiaTests` or `ctest --test-dir build`

### Target-Specific Scripts
- **Web:** `scripts/build_web.sh` (Requires Emscripten SDK).
- **Android:** `scripts/build_android.sh` (Requires Android NDK/SDK).

## Project Architecture
The codebase is organized into modular components under `src/`:

- **`core/`**: Central game loop (`main.cpp`), save system, and the critical **Astrological Clock** (`clock.h/cpp`).
- **`world/`**: Map logic, player movement, enemy AI, and harvestable plant nodes.
- **`lab/`**: Alchemy stations, processing timers, and the residue purification state machine.
- **`alchemy/`**: Plant database, spagyric product definitions, and the "Seven Oblations" progression system.
- **`combat/`**: Sigil-drawing recognition and on-map combat resolution.
- **`economy/`**: Customer request generation and town vendor logic.
- **`ui/`**: HUD, grimoire-style Plant Compendium, and inventory.

## Development Conventions

### Coding Style & Standards
- **Standard:** Strictly adhere to **C++17**.
- **Naming:** Use descriptive variable and function names. Avoid single-letter variables except for loop counters.
- **Documentation:** Every file should have a clear section header explaining its purpose. Use inline comments for non-obvious logic, particularly for planetary math and alchemical state machines.
- **Header Guards:** Use `#ifndef FILENAME_H` style guards.

### Testing Practices
- **Framework:** `doctest`.
- **Location:** All tests are located in the `tests/` directory, mirroring the `src/` structure.
- **Mandate:** New features should include corresponding unit tests in `tests/`.

### Alchemical Integrity
The game mechanics must align with operative spagyric principles as documented in the GDD (`Flora_Philosophia/Flora_Philosophia_GDD.md`). This includes the Ternary system (Mercurius, Sulphur, Sal) and the specific planetary attributions from Culpeper's Herbal.
