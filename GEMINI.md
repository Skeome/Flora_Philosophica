# Flora Philosophia - Project Overview & Context

This document provides foundational context and instructions for AI agents interacting with the Flora Philosophia codebase.

## 1. Project Identity

Flora Philosophia is a C++17 RPG and alchemy simulator built with Raylib. It functions as a "stealth-educational" tool for real-world operative spagyrics (plant alchemy).

*   **Core Hook:** In-game processes (maceration, calcination, leaching) accurately reflect real-world herbal spagyric preparation.
*   **Key Mechanic:** A real-time **Astrological Engine** that synchronizes game events and harvest quality with the player's local real-world planetary hours.

## 2. Technical Stack

*   **Language:** C++17 (standardized on clean, readable code for educational purposes).
*   **Rendering:** [Raylib](https://www.raylib.com/) (cross-platform 2D/3D).
*   **Build System:** CMake (v3.20+).
*   **Dependencies:**
    *   `nlohmann/json` (Data serialization).
    *   `doctest` (Unit testing).
    *   `CPM.cmake` (Package management).
*   **Targets:** Native (Linux/Windows), Web (Emscripten/WebAssembly), Android (NDK).

## 3. Project Structure

```text
/
├── assets/                  # Sprites, backgrounds, audio, and JSON data
├── Flora_Philosophia/       # Design documents (GDD) and Obsidian vault
├── scripts/                 # Build and run automation
├── src/                     # Source code
│   ├── main.cpp             # Application entry point
│   ├── core/                # Astrological clock, save systems, engine logic
│   ├── world/               # Maps, Player, Inventory, Item placement
│   ├── lab/                 # Laboratory stations and processing timers
│   ├── alchemy/             # Plant database and spagyric logic
│   ├── combat/              # Sigil-based combat system
│   ├── economy/             # Customer requests and mailbox
│   ├── idle/                # Terrariums and passive generation
│   └── ui/                  # HUD, inventory panels, and compendium
├── tests/                   # Unit tests (mirrors src/ structure)
└── CMakeLists.txt           # Main build configuration
```

## 4. Development Workflow

### Building and Running

*   **Native Build:**
    ```bash
    cmake -B build && cmake --build build
    ./scripts/run_game.sh
    ```
*   **Running Tests:**
    ```bash
    cd build
    ctest --output-on-failure
    # OR run the binary directly
    ./FloraPhilosophiaTests
    ```
*   **Web Build:** Use `scripts/build_web.sh` (requires Emscripten).
*   **Android Build:** Use `scripts/build_android.sh` (requires Android NDK).

### Coding Standards

*   **Readability:** Prioritize clear, descriptive variable and function names over brevity.
*   **Documentation:** Every file should have a section header explaining its purpose. Use inline comments for complex alchemical or mathematical logic.
*   **Modularity:** Keep game systems decoupled. UI should not directly mutate world state; use the `Inventory` and `RoomManager` interfaces.
*   **Types:** Prefer explicit types over `auto` unless the type is redundant or extremely complex (e.g., iterators).

## 5. Core Systems Context

### The Astrological Engine (`src/core/clock.h`)
Calculates the current planetary hour based on the player's latitude, longitude, and UTC time. This affects plant harvest quality (Pristine vs. Debased) and lab success rates.

### The Residue Pipeline (`src/lab/`)
A core educational loop:
1.  **Spent Plant (Residue)** -> 2. **Caput Mortuum** (Scorched) -> 3. **Calx** (Ash) -> 4. **Alkali** (Leached) -> 5. **Salt** (Purified).
Players must learn not to discard the "Body" (residue) in the compost bin.

### Item Placement & Interaction (`src/world/`)
Players can place furniture and lab apparatus in the sanctuary. Interaction is handled via the `E` key near objects, managed by `RoomManager`.

## 6. Current Goals & TODOs

*   [ ] Implement the full Apparatus UI for lab stations.
*   [ ] Integrate JSON-based plant database for all harvestable nodes.
*   [ ] Expand the Save/Load system using `nlohmann/json`.
*   [ ] Implement Sigil drawing recognition for combat.

---
*Note: This file is a living document. Update it as architectural patterns or major milestones evolve.*
