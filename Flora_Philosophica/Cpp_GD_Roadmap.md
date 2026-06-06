# Technical Architecture and Implementation Feasibility Analysis of a Geocoded Alchemical Apothecary Simulator in Modern C++17

Developing a cross-platform simulation game using native C++17 presents a sophisticated set of engineering challenges for an independent developer. Unlike managed, commercial game engines that package heavy runtimes and generalized execution layers, using a minimalist multimedia framework like Raylib forces the developer to implement critical systems—including spatial databases, collision systems, asset lifecycles, UI layouts, and platform-specific hardware abstractions—from scratch.

Flora Philosophica serves as an ideal case study for this architectural paradigm. The game combines the tile-based simulation loop of classic sandbox RPGs with a real-time, geolocated astrological engine that coordinates gameplay states with physical coordinates and real-world planetary cycles. This technical report examines the subsystem architecture, mathematical and astrological calculations, WebAssembly compilation mechanics, and Android native integration strategies required to build and deploy such a simulator as an independent native developer.

## Architectural Design and Solo Developer Project Organization

Managing a codebase of significant scale presents a substantial organizational hurdle for an independent developer. The codebase of Flora Philosophica consists of 197 directories and 1549 files, a structural footprint that can easily lead to architectural decay and scope creep if not managed through a strict, decoupled design. To maintain control over this complexity, the repository incorporates modular coding folders alongside dedicated inline game design documentation.

```
.
├── assets/                     # Platform-agnostic static binary assets
│   ├── audio/
│   ├── backgrounds/
│   ├── data/
│   ├── fonts/                  # Optimized Unicode alchemical typography
│   └── sprites/
├── Flora_Philosophica/         # Conceptual and game design documentation
│   ├── Flora Philosophica.canvas # Visual obsidian design board mapping systems
│   ├── Flora_Philosophica_GDD.md # Master Game Design Document
│   ├── The Cabin/              # Location-specific mechanical design files
│   │   ├── Cabin.md
│   │   ├── Laboratory.md
│   │   └── Storefront.md
│   └── The Ternary Alchemical Order/
│       └── The Ternary Alchemical Order.md
├── src/                        # Core source tree (C++17 standard)
│   ├── alchemy/                # Spagyric database and formulation logic
│   ├── combat/                 # Placeholder scaffold for combat loops
│   ├── core/                   # Astrological clock and system clock managers
│   ├── economy/                # Placeholder scaffold for merchant mechanics
│   ├── idle/                   # Placeholder scaffold for background extraction
│   ├── lab/                    # Placeholder scaffold for lab apparatuses
│   ├── ui/                     # Interface systems and screenspace rendering
│   └── world/                  # Grid maps, pathfinding, and entity management
├── tests/                      # Testing modules mapped to source subsystems
└── scripts/                    # Platform target compilation wrappers
```

The game uses a strict separation of concerns, isolating platform-independent simulation state from platform-specific rendering layers. While directories like `combat`, `economy`, `idle`, and `lab` exist as scaffolding in both the source and testing trees, they represent planned modular expansions to prevent structural debt as the simulator grows.

The developer manages these subsystems by linking them to conceptual blueprints stored directly in the repository under the `Flora_Philosophica` documentation tree. The master file `Flora_Philosophica_GDD.md`, the visual mapping file `Flora Philosophica.canvas`, and location-specific specifications (such as `Laboratory.md` and `Storefront.md`) define the architectural rules and dependency boundaries for each module before code is written.

To compile these modules efficiently, the project utilizes a unified CMake build system configured with CPM.cmake (CMake Package Manager). This system automates the fetching, locking, and compilation of external dependencies (such as Raylib, Doctest, and Nlohmann JSON) directly from source control. This setup ensures a reproducible build environment across macOS, Linux, Windows, WebAssembly, and Android targets.

| Compilation Subsystem | Primary C++ Header Path | Mapped Testing File Path | Target Output Layer | External Dependencies |
|---|---|---|---|---|
| Astrological Core | `src/core/clock.h` | `tests/core/clock_test.cpp` | UTC Clock HUD & Solar Calculations | `<chrono>`, Geolocation API |
| World Navigation | `src/world/map.h` | `tests/world/map_test.cpp` | Room Coordinates & Movement Maps | Raylib Camera2D Matrices |
| Alchemy Engine | `src/alchemy/plant_db.h` | Mapped via Plant Node tests | Taxonomic Database & Formulation | nlohmann/json parsing library |
| Apothecary Lab | Scaffolded under `src/lab/` | Scaffolded under `tests/lab/` | Apparatus State & Processing loops | SQLite serialization backend |
| User Interface | `src/ui/inventory_ui.h` | Mapped via rendering loop | Screenspace Overlays & Mobile HUD | Raylib Gestures and Textures |

The Core Game Loop and Player Simulation MechanicsAt the center of Flora Philosophica lies a unified execution loop located in `main.cpp`. The viewport is configured to a locked 1200x900 pixel canvas operating at a target rate of 60 frames per second. The game world is built on a 2D grid structure using a uniform tile size of 60 pixels, allowing for efficient coordinate math and rapid spatial lookups.

When a session initializes, the engine instantiates the `RoomManager`, spawning the player character at coordinate (600, 450) on the exterior cabin map. The player’s spatial position is updated continuously within the active room’s boundary limits, and a Raylib `Camera2D` component centers dynamically on the player to scroll the map smoothly.

```

                                  │
                                  ▼
                    [Check Movement Input Vector]
                                  │
                                  ▼
              
                                  │
                                  ▼
                 [Apply Velocity to Player Position]
                                  │
                                  ▼
              
```

To support multiple platforms, the player controller implements a hybrid input layer. On desktop computers, players move using traditional keyboard keys (WASD/Arrows) and interact with the environment using the mouse. Clicking the mouse triggers an A* pathfinding algorithm that navigates the player to world coordinates, rendering debugging lines to visualize the path during development.

On mobile devices, the input layer dynamically emulates a virtual touch joystick. When the player drags on a touchscreen, the engine calculates the touch offset from the initial touch point using vector arithmetic and draws two concentric circles to represent the joystick state.

```C++
// Drag-and-drop joystick emulation vector calculations
Vector2 dragDifference = Vector2Subtract(currentTouchPosition, initialTouchPosition);
float dragDistance = Vector2Length(dragDifference);
if (dragDistance > maxJoystickRadius) {
    // Clamp joystick input to maximum boundary limits
    dragDifference = Vector2Scale(Vector2Normalize(dragDifference), maxJoystickRadius);
}
```

The game world is populated with dynamic, interactable entities managed by the `RoomManager`. The player can trigger contextual actions by pressing the `[E]` key or tapping nearby objects. The interaction system uses radial proximity checks to identify valid targets.

```
                  [Check Proximity to Active Entities]
                                   │
         ┌─────────────────────────┼─────────────────────────┐
         ▼                         ▼                         ▼
  [Placed Items]             [Apparatuses]             [Plant Nodes]
- Inspect attributes       - Configure heaters       - Query clock state
- Rotate placement orientation - Process raw herbs   - Perform harvest calculation
- Relocate decor structure - Compost spent residue   - Evaluate item quality
```

The placement engine supports an active decoration mode. When a player chooses to place furniture or laboratory gear, the engine renders a semi-transparent "ghost" preview of the item snapped to the nearest 60-pixel grid intersection. The player presses Left-Click to finalize placement or the `[B]` key to cancel the operation and return the item to the inventory stack.

All interaction events and feedback logs are pushed to a central message queue displayed at the top of the viewport. This HUD component automatically wraps long text lines and truncates older logs using FIFO indexing, preventing text overlap during active play sessions.

## The Astrological Engine and Mathematical Solar Calculation

The astrological engine is the core calculation system in Flora Philosophica, driving both gameplay events and item quality calculations. This engine determines the active planetary hour by resolving the current UTC time against the player's real-world coordinates.

To compute these values, the application queries the system clock to retrieve the current Unix timestamp. The astronomical formulas require converting this standard timestamp into a local Julian Date ($JD$) representation:

$$JD = \text{Day} - 32075 + 1461 \times \frac{Y}{4} + 980 \times \frac{M}{3} - 2 \times \frac{M}{12}$$

Where $Y$ and $M$ represent the year and month adjusted to account for leap years.

The engine uses this Julian Date to calculate the Sun's anomaly and declination, producing the exact sunrise and sunset times for the player's coordinate location.

```C++
// Astrological clock engine interface declaration
namespace FloraPhilosophica::Core {
    enum class Planet { Saturn = 0, Jupiter = 1, Mars = 2, Sun = 3, Venus = 4, Mercury = 5, Moon = 6 };

    struct PlanetaryHourInfo {
        Planet rulingPlanet;
        Planet dayRuler;
        int hourIndex;            // Ranging 0-11 for day, 12-23 for night
        double minutesRemaining;
        std::string planetName;
        long long hourStartUtc;
        long long hourEndUtc;
    };

    class AstrologicalClock {
    public:
        PlanetaryHourInfo CalculatePlanetaryHour(double latitude, double longitude, long long utcTimestamp);
        static std::string GetPlanetName(Planet planet);
    private:
        struct SolarTimes { double sunriseUTC; double sunsetUTC; bool isPolar; };
        struct DayLimits { long long sunriseUnix; long long sunsetUnix; bool isPolar; };

        SolarTimes CalculateSolarLimits(double latitude, double longitude, double julianDate);
        DayLimits GetDayLimits(double latitude, double longitude, long long utcTimestamp, int dayOffset);
    };
}
```

```

                                  │
                                  ▼
            
                                  │
                                  ▼
          
                                  │
                                  ▼
          
                                  │
                                  ▼
     
                                  │
                                  ▼
             
                                  │
         ┌────────────────────────┴────────────────────────┐
         ▼                                                 ▼
   (If Day Segment)                                (If Night Segment)
- Range: Sunrise to Sunset                      - Range: Sunset to next Sunrise
- Slice Length = (Sunset - Sunrise) / 12        - Slice Length = (next Sunrise - Sunset) / 12
- Hour Index = Floor(Delta / Slice)             - Hour Index = Floor(Delta / Slice) + 12
         │                                                 │
         └────────────────────────┬────────────────────────┘
                                  │
                                  ▼
           
                                  │
                                  ▼
         
                                  │
                                  ▼
             [Output: Active Planetary Hour Context]
```

To calculate the astrological parameters correctly, the engine must handle planetary day boundaries. In classical astrology, a planetary day does not begin at midnight; it starts at the exact moment of local sunrise.

If a player is active at 2:00 AM on a Tuesday, the astrological day is still considered Monday because Tuesday's sunrise has not yet occurred. The engine handles this overnight overlap by using a relative dayOffset (-1, 0, or 1) inside GetDayLimits to calculate and compare solar limits across consecutive days.

* **Yesterday**(`dayOffset = -1`): Calculated to determine if a late-night timestamp belongs to the previous astrological day's night segment.
* **Today**(`dayOffset = 0`): Computed as the baseline solar reference for the current calendar date.
* **Tomorrow**(`dayOffset = 1`): Computed to determine the next day's sunrise, which marks the end of the current day's night segment.

Once the solar limits are resolved, the day is split into 24 planetary hours: 12 daytime hours (indices 0–11) and 12 nighttime hours (indices 12–23). The ruling planet is determined by applying the hour index to the Chaldean sequence, offset by the current weekday ruler:

$$\text{Ruling Planet Index} = (\text{WeekdayRulerIndex} + \text{HourIndex}) \bmod 7$$

This ruling planet directly impacts item quality when harvesting plants. If the ruling planet of the current hour matches the taxonomic ruler of the plant, the player receives a pristine harvest bonus, which is logged to the HUD message feed.

## Technical Asset Pipelines and Font Optimization

For a native game compiled for multiple platforms, managing memory footprint and load times is critical. Loading complete, high-resolution font files into graphic memory wastes resources, particularly on web runtimes where assets must be pre-packaged or downloaded on the fly.

*Flora Philosophica* addresses this resource cost by implementing an optimized font-loading system in `fonts.h`. The Archemy OpenType Font (`Archemy.otf`) is used to display planetary glyphs, alchemical processes, and HUD indicators. To minimize graphics card memory usage, the engine uses a selective Unicode rasterization pipeline.

```
                     [Execute LoadAlchemyFont()]
                                  │
                                  ▼
           [Open Font file from Path via std::fopen("rb")]
                                  │
         ┌────────────────────────┴────────────────────────┐
         ▼                                                 ▼
   (If File Missing)                               (If File Exists)
- Log TraceLog (LOG_WARNING)                      - Close File handle immediately
- Fallback: Return GetFontDefault()               - Execute Raylib LoadFontEx()
                                                  - Pass specific Codepoints Array
                                                  - Pass size parameter (28 pt HUD)
                                                  - Log TraceLog (LOG_INFO)
                                                  - Return compact Texture Atlas
```

The application defines an array of eight specific Unicode codepoints to load for the planetary hours:

| Unicode Code Point | Graphical Character | Astrological Meaning | Target UI Element |
|---|---|---|---|
| `0x2609` | ☉ | Solis | Solar Hour Icon |
| `0x263D` | ☽ | Lunae | Lunar Hour Icon |
| `0x2642` | ♂ | Martis | Martial Hour Icon |
| `0x263F` | ☿ | Mercurii | Mercurial Hour Icon |
| `0x2643` | ♃ | Jovis | Jovian Hour Icon |
| `0x2640` | ♀ | Veneris | Venusian Hour Icon |
| `0x2644` | ♄ | Saturni | Saturnian Hour Icon |

By passing this array of codepoints to Raylib’s `LoadFontEx`, the engine generates a tiny, optimized texture atlas in graphic memory containing only the necessary glyphs.

To prevent asset loading errors from crashing the game, the font loader runs a validation check using standard C file APIs. If the font file is missing, the game logs a warning using Raylib’s `TraceLog` and falls back to the default system font, ensuring the game remains playable on misconfigured systems.

## Build Automation and Cross-Platform Target Deployment

To support multiple platforms as a solo developer, *Flora Philosophica* uses CMake to automate builds for desktop, mobile, and web targets. This automation is managed by build scripts in the root directory: `scripts/run_game.sh` for desktop testing, `scripts/build_web.sh` for WebAssembly builds, and `scripts/build_android.sh` for Android packages.

```

                                         │
        ┌────────────────────────────────┼────────────────────────────────┐
        ▼                                ▼                                ▼
                            
- Compile C++17 sources         - Run emcc compiler toolchain    - Compile native ARM library
- Link system GLFW libraries    - Apply -sWASMFS filesystem      - Link NDK native_app_glue
- Set local resource directories- Embed assets into binary data  - Sign package into native APK
```

### The WebAssembly Emscripten Interface

Compiling C++17 code for web browsers using the Emscripten toolchain requires resolving execution loop and storage system conflicts. Browsers run on a single-threaded cooperative event loop. A standard synchronous infinite loop (`while (!WindowShouldClose)`) blocks this thread, causing the browser tab to hang.

To prevent this, the engine uses Emscripten's main loop registry to run the frame update logic asynchronously.

```C++
#if defined(PLATFORM_WEB)
#include <emscripten.h>

void EmscriptenLoopCallback(void* arg) {
    auto* gameManager = static_cast<GameManager*>(arg);
    gameManager->UpdateFrame();
}
#endif

void GameManager::Run() {
    #if defined(PLATFORM_WEB)
        // Hand frame execution over to the browser's cooperative main loop
        emscripten_set_main_loop_arg(EmscriptenLoopCallback, this, 0, 1);
    #else
        while (!ShouldWindowClose()) {
            UpdateFrame();
        }
    #endif
}
```

The web version uses the WASMFS filesystem architecture with the Origin Private File System (OPFS) backend to manage game saves. Unlike older systems like IDBFS, which rely on slow, asynchronous JavaScript callbacks, WASMFS allows standard C++ file streams (`std::ofstream` and `std::ifstream`) to write directly to persistent sandboxed storage synchronously.

The OPFS backend must be mounted before the Emscripten main loop is registered. WASMFS creates background promises that need the browser event loop to resolve; registering the loop first can cause these initialization promises to deadlock.

```C++
#ifdef __EMSCRIPTEN__
#include <emscripten/wasmfs.h>

void InitializePersistentOPFS() {
    std::string mountPoint = "/offline_apothecary";
    backend_t opfsBackend = wasmfs_create_opfs_backend();
    
    // Mount the sandboxed persistent storage partition
    int result = wasmfs_create_directory(mountPoint.c_str(), 0777, opfsBackend);
    if (result == 0 || result == EEXIST) {
        TraceLog(LOG_INFO, "OPFS storage interface successfully configured.");
    }
}
#endif
```

WebAssembly requires absolute virtual paths (e.g., `/offline_apothecary/save.sz`), while desktop targets use relative paths (e.g., `saves/save.sz`). The engine uses conditional compilation macros to resolve paths correctly at compile time.

To guarantee that save files are written to the browser's storage before the tab is closed, the game registers listeners for browser events like `visibilitychange` and `beforeunload`, flushing any unwritten memory buffers to disk immediately.

### The Android NativeActivity Integration

Deploying the C++ engine to Android devices without writing a heavy Java UI framework requires using the `NativeActivity` class. The build system links the game against the static `android_native_app_glue` helper library, which spawns a background worker thread to process input events and rendering loops without blocking the main UI thread.

```C++
#include <android_native_app_glue.h>

void android_main(struct android_app* state) {
    // Prevent linker optimization from stripping the app glue entry point
    app_dummy();
    
    GameManager game(state);
    game.Initialize();
    game.Loop();
}
```

Android does not provide native C++ APIs to query GPS hardware. To get the player's real-world coordinates for the astrological engine, the game subclasses NativeActivity in Java to listen to location updates, passing the coordinates to C++ over the Java Native Interface (JNI).

```Java
package com.apothecary.floraphilosophica;

import android.app.NativeActivity;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;

public class ApothecaryActivity extends NativeActivity implements LocationListener {
    // Declare JNI link to native C++ update method
    public native void updateNativeLocation(double lat, double lon);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        LocationManager lm = (LocationManager) getSystemService(LOCATION_SERVICE);
        try {
            lm.requestLocationUpdates(LocationManager.GPS_PROVIDER, 5000, 10, this);
        } catch (SecurityException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onLocationChanged(Location location) {
        // Send coordinates back to the native C++ thread
        updateNativeLocation(location.getLatitude(), location.getLongitude());
    }
}
```

The C++ JNI bridge receives the location updates and caches the coordinates safely in the game's execution context.

```C++
#include <jni.h>

extern "C" {
    JNIEXPORT void JNICALL
    Java_com_apothecary_floraphilosophica_ApothecaryActivity_updateNativeLocation(
        JNIEnv* env, jobject obj, jdouble lat, jdouble lon) {
        
        // Cache coordinates for the astrological engine
        GameClock::SetLocation(static_cast<double>(lat), static_cast<double>(lon));
    }
}
```

Android assets (such as sprite textures and font files) are packed directly inside the compressed `.apk` package. Because standard C++ file APIs cannot read files inside compressed archives, the game uses Raylib's platform-aware asset loaders.

Raylib queries the NDK’s AAssetManager using the context from GetAndroidApp(), loading assets directly from the APK and removing the need for manual file extraction.

| Architectural Domain | Desktop (macOS/Win/Linux) | WebAssembly (Emscripten) | Mobile (Android NDK) |
|---|---|---|---|
| Main Execution Loop | Blocking infinite loop | Event-driven callback | Background glue worker thread |
| System I/O & Storage | Relative paths (`saves/`) | Mapped OPFS sandbox directory | Packed app sandbox storage |
| Coordinates Access| Fixed coordinates configuration | Geolocation Web API | JNI Java Location Service link |
| Asset Resolution | Local disk path strings | Virtual preloaded binary data | NDK AAssetManager query |
| Binary Output Format | Native machine executable | WebAssembly binary (`.wasm`) | Android package archive (`.apk`) |

## Architectural Feasibility Assessment

Building Flora Philosophica with modern C++17 and Raylib is highly feasible, offering significant performance and code control advantages for an independent developer. However, succeeding with this native approach requires carefully managing development overhead and targeting compilation complexity.

```

                                     │
           ┌─────────────────────────┴─────────────────────────┐
           ▼                                                   ▼
   [Core Logic Layer]                                  [Platform Adapters]
- Game Simulation Loops                         - Web: Emscripten Loop & WASMFS 
- Astrological Equations                  - Mobile: JNI Bridges & Android Glue [4]
- Plant Database Models                         - Desktop: Standard Native Build 
```

By keeping the core simulation logic strictly separated from platform-specific adapters, the developer can build and test systems independently. The game rules, plant databases, and astrological math can be compiled and validated in fast, local command-line tests, bypassing the need to run full builds on mobile devices or inside browsers for every code change.This decoupled structure allows an independent developer to handle the scale of a multiplatform codebase, delivering a unique gameplay experience built on an optimized, native framework.