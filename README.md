# Flora Philosophica

An alchemical apothecary sanctuary game, now powered by **Godot 4.6+** and **GDExtension**.

## Architecture Shift
The project has migrated from a standalone Raylib executable to a Godot GDExtension. The core game logic (Alchemical Clock, Inventory, Item definitions) is written in C++ and compiled into a Shared Object (`.so`) that Godot loads at runtime.

## Prerequisites
To build and run this project on **Arch Linux**, you need:

- **Godot Engine 4.6+** (available as `godot` in your PATH)
- **CMake 3.20+**
- **C++17 Compiler** (GCC or Clang)
- **Python 3** (required by `godot-cpp` for binding generation)
- **Make** or **Ninja**

## Building the GDExtension

The build process uses CMake and handles dependencies (like `godot-cpp` and `nlohmann_json`) automatically via CPM.cmake.

1.  **Generate Build Files:**
    ```bash
    mkdir -p build
    cd build
    cmake ..
    ```

2.  **Compile:**
    ```bash
    make -j$(nproc)
    ```

The compiled library will be automatically placed in `godot/bin/libflora_philosophica.so`.

## Running the Game

### Via Godot Editor
Open the `godot/project.godot` file in the Godot Editor. The GDExtension will load automatically.

### Via Command Line (Headless/Exported)
To run the project directly using the Godot binary:
```bash
godot --path godot
```

## Project Structure
- `src/`: C++ Source code for the GDExtension.
    - `core/`: Core systems like the `PlanetaryHourCalculator`.
    - `world/`: Inventory and Item logic.
- `godot/`: The Godot project directory.
    - `bin/`: Contains the `.gdextension` manifest and the compiled `.so` library.
- `Flora_Philosophica/`: Design documents and alchemical references.

## Using the Native Classes in GDScript
Classes registered in C++ are available directly in Godot. For example, to use the Planetary Hour Calculator:

```gdscript
var calculator = PlanetaryHourCalculator.new()
var info = calculator.calculate_planetary_hour(lat, lon, Time.get_unix_time_from_system())
print(info.planet_name)
```
