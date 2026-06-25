# 🌿 Flora Philosophica

A cozy alchemical apothecary sanctuary game — and a stealth-educational guide to real-world **spagyric** plant alchemy. Forage, cultivate, and craft herbal preparations guided by a real-time astrological engine. A player who follows the in-game processes can actually produce real herbal spagyric preparations in their own kitchen.

Built with **Godot 4.7** and a **C++17 GDExtension** backend. Inspired by Nicholas Culpeper's *Complete Herbal* (1652) and the Western tradition of astrological herbalism.

---

## ✨ Features

### Real-Time Planetary Hour Engine
A C++ astronomical engine calculates true planetary hours from GPS coordinates and system clock. Sunrise and sunset are derived from solar position algorithms (Julian date, equation of time, solar declination, hour angle), dividing day and night into 12 planetary hours each following the Chaldean order (Saturn → Jupiter → Mars → Sun → Venus → Mercury → Moon). Planetary hours directly influence crafting potency, harvest quality, and atmospheric effects.

Default observer location: Medford, Oregon (42.3265°N, 122.8756°W) — overridden by GPS on mobile.

### VSOP87 Celestial Mechanics
A clean-room implementation of the VSOP87 planetary theory provides true geocentric ecliptic coordinates for all 7 classical planets. This powers the main menu's "dance of the spheres" — an animated orrery with real retrograde loops, where Venus traces her 8-year pentagram and Saturn completes his 29.45-year circuit.

### The Spagyric Pipeline
A historically accurate 9-step alchemical process — the core gameplay loop:

1. **Fresh → Dried** — Drying Rack (1 hr)
2. **Dried → Ground** — Mortar & Pestle (5 min)
3. **Ground → Tincture + Spent Residue** — Maceration Jar (1 day)
4. **Residue → Calx Black** — Furnace (30 min)
5. **Calx Black → Powdered Calx Black** — Mortar & Pestle (5 min)
6. **Powdered Calx Black → Calx Grey** — Leaching Dish (1 hr)
7. **Calx Grey → Calx White** — Furnace (30 min)
8. **Calx White → Salt Standard** — Furnace (30 min)
9. **Salt Standard → Purified Salt** — Leaching Dish (1 hr)

The "compost bin trap" — players initially discard the spent residue from Step 3, not realizing it contains the valuable Salt principle needed for the higher stages.

### Astrological Harvest Quality
Plant quality is determined by planetary alignment at the moment of harvest:

| Quality | Condition |
|---|---|
| **Celestial** | Day ruler AND hour ruler match the plant's planet |
| **Pristine** | Hour ruler matches |
| **Standard** | Neutral alignment |
| **Stressed** | Hour ruler is the opposing planet |
| **Debased** | Both day AND hour are opposing planets |

### Plant Database
100+ herbs sourced from Culpeper's *Complete Herbal*, each with historically accurate planetary ruler, classical element, and medicinal properties. The C++ `PlantDatabase` class alone spans 68 KB.

### Apparatus & Stations
19 apparatus types available for placement across 4 rooms (Exterior, Cabin Main, Cabin Loft, Garden): Fireplace, Drying Rack, Mortar & Pestle, Maceration Jar, Compost Bin, Work Bench, Copper Alembic, Glass Flask, Glassblowing Station, Distillation Train, Soxhlet Extractor, Pelican Flask, Retort Train, Terrarium, Bookshelf, Storage Chest, Mailbox Post, Furnace, and Leaching Dish. Each has tile dimensions, tier requirements, and indoor/outdoor placement flags.

### Overworld Exploration
Pixel-art tile-based overworld (1920×1080 world canvas at 16px tiles, rendered at 640×360 native resolution) with y-sorted depth rendering. Walk freely with **WASD** or **click-to-move** (tap-to-move on mobile). Scene transitions via door zones connect multiple areas. The player character, "Basil," has directional idle and walk animations.

### Combat & Meditation
Physical combat does not exist in the overworld. Combat is initiated via **Meditation ("Guided Imagery")** at specific nodes, entering the "Spirit World." Here, players fight spiritual essences of monsters and inner shadow clones (akin to Dark Link). Combat relies on drawing sigils via touch input, with the current Planetary Hour providing massive damage multipliers (up to 4x for matching Day & Hour). Defeat results in "Mental Fatigue," lowering laboratory crafting success.

### Character Progression: The Seven Oblations
No traditional EXP bar. Every crafted spagyric product acts as an XP book for its ruling planet's RPG stat. The ultimate goal is crafting the reusable **Plant Stone**. Consuming an Oblation during matching celestial windows grants permanent stat boosts; consuming it out of alignment yields temporary buffs or even debuffs.

### Economy & The Ternary Alchemical Order
Sell products via the Mailbox Post (early game) or your own Storefront (late game). Pricing scales dynamically based on base value, harvest quality, and customer affluence. Elite customers include Wizards, Nobles, and disguised members of **The Ternary Alchemical Order**, who send action-triggered cryptic letters (sealed with wax) to guide players toward advanced operative alchemy.

### Persistence
Full save/load system (JSON to `user://save.json`) tracking:
- 46-slot inventory (10 hotbar + 36 grid)
- Placed apparatus across 4 rooms
- Player position
- Planted herbs and growth stages
- Timestamps

### Real-Time Design
The pause menu intentionally does **not** pause the SceneTree — planetary hours continue ticking and lab apparatus timers keep running, reinforcing the real-time nature of the alchemical process.

---

## 🏗️ Architecture

Core systems are written in **C++17** and compiled as a GDExtension shared library. Game logic and UI are in **GDScript**. The C++ layer registers **13 classes** into Godot's ClassDB:

| Category | Classes |
|---|---|
| **Core** | `PlanetaryHourCalculator`, `PlanetaryOrbitCalculator` |
| **World** | `HarvestItem`, `ItemDefinition`, `ItemDB`, `InventorySlot`, `Inventory`, `PlacedItem`, `RoomManager` |
| **Alchemy** | `PlantData`, `PlantDatabase`, `Recipe`, `Station` |

### Project Structure

```
Flora_Philosophica/
├── src/                            # C++17 GDExtension source
│   ├── core/
│   │   ├── clock                       # Solar position & planetary hours
│   │   ├── orbit_calculator            # VSOP87 geocentric coordinates
│   │   ├── vsop87_terms.h              # Orbital coefficient tables
│   │   └── register_types.cpp          # 13-class GDExtension registration
│   ├── world/
│   │   ├── item                        # HarvestItem (22 stages, 5 quality tiers)
│   │   │                               #   + ItemDefinition + ItemDB (19 types)
│   │   ├── inventory                   # 46-slot inventory (10 hotbar + 36 grid)
│   │   ├── placed_item                 # Apparatus logic, timed processing
│   │   └── room_manager               # 4 rooms: Exterior, Cabin, Loft, Garden
│   ├── alchemy/
│   │   ├── plant_db                    # PlantDatabase — 100+ Culpeper entries (68 KB)
│   │   ├── recipe                      # Spagyric pipeline definitions
│   │   └── station                     # Processing nodes with real-time tick
│   ├── entry.cpp                       # GDExtension entry point
│   ├── main.cpp                        # Legacy Raylib entry (not compiled)
│   ├── combat/  economy/  idle/  lab/  # Scaffolded — reserved for future systems
│   └── ui/inventory_ui                 # Legacy Raylib UI (not compiled)
│
├── godot/                          # Godot 4.7 project (GL Compatibility)
│   ├── scenes/
│   │   ├── main_menu.tscn              # Title screen w/ VSOP87 planet orbits
│   │   ├── main.tscn                   # Overworld (Ground, Walls, Rooftops,
│   │   │                               #   Counter-tops, Player, HUD)
│   │   ├── player.tscn                 # "Basil" — CharacterBody2D, 4-dir anims
│   │   ├── plant_node.tscn             # Harvestable plant (quality from alignment)
│   │   ├── clock_hud.tscn              # Planetary hour display (♄♃♂☉♀☿☽)
│   │   └── pause_menu.tscn             # Non-pausing overlay (real-time design)
│   ├── scripts/
│   │   ├── game_manager.gd             # Autoload: save/load, C++ object refs
│   │   ├── player.gd                   # Movement, animation, input
│   │   ├── world.gd                    # Door transitions, auto-save
│   │   ├── plant_node.gd               # Growth stages, astrological quality
│   │   ├── recipe_db.gd                # 9-step spagyric pipeline
│   │   ├── clock_hud.gd                # Planetary hour HUD w/ planet glyphs
│   │   ├── main_menu.gd                # Menu logic, TTAO panel, credits, links
│   │   ├── pause_menu.gd              # Pause/resume, save, load, quit
│   │   └── planet_orbit_animator.gd    # VSOP87 orbit visualization (377 lines)
│   ├── main.gd                         # Clock display (planetary hour ticker)
│   ├── assets/
│   │   ├── sprites/                    # character/ items/ npc/ plants/
│   │   │                               #   stations/ tiles/ world/
│   │   ├── fonts/Archemy.otf           # Custom alchemical Unicode font
│   │   ├── ui/                         # Menu art, planet PNGs, UI theme
│   │   ├── music/                      # (planned)
│   │   └── sounds/                     # (planned)
│   └── bin/                            # Compiled .so + .gdextension manifest
│
├── Flora_Philosophica/             # Obsidian vault — design documents
│   ├── Flora_Philosophica_GDD.md       # Master GDD v0.2 (36.6 KB)
│   ├── Cpp_GD_Roadmap.md              # Technical architecture roadmap
│   ├── The Cabin/                      # Location design (Cabin, Lab, Storefront)
│   ├── The Ternary Alchemical Order/   # Faction lore
│   ├── Plants/                         # Per-plant documentation (planned)
│   ├── TAO_Seal.png                    # Faction seal artwork
│   └── Flora Philosophica.canvas       # Obsidian design board
│
├── tests/                          # Test scaffolding (mirrors src/ structure)
├── scripts/                        # Build & run helpers
│   ├── build_android.sh                # Android cross-compilation
│   ├── build_web.sh                    # WebAssembly compilation
│   └── run_game.sh                     # Wayland/Hyprland compatibility wrapper
├── Culpepers_Complete_Herbal.epub  # Primary botanical reference source
├── CMakeLists.txt                  # CMake + CPM build system
└── save.json                       # Legacy save data (pre-Godot format)
```

---

## 🎯 Platform Targets

| Platform | Status | Notes |
|---|---|---|
| **Linux** | Primary | Developed on Arch Linux w/ Hyprland/Wayland |
| **Windows** | Supported | CMake produces `.dll` |
| **macOS** | Supported | CMake produces `.dylib` |
| **Android** | Build script ready | `scripts/build_android.sh` |
| **WebAssembly** | Build script ready | `scripts/build_web.sh` |

---

## 🔧 Prerequisites

- **Godot Engine 4.7+**
- **CMake 3.20+**
- **C++17 compiler** (GCC or Clang)
- **Python 3** (required by `godot-cpp` for binding generation)
- **Make** or **Ninja**

## Building the GDExtension

CMake handles dependencies automatically via [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake):
- `godot-cpp` 10.0.0-rc1
- `nlohmann_json` 3.11.3

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

The compiled library is placed at `godot/bin/libflora_philosophica.so`.

## Running the Game

**Godot Editor** — Open `godot/project.godot`. The GDExtension loads automatically.

**Command line:**
```bash
godot --path godot
```

---

## 📜 Using Native Classes in GDScript

C++ classes registered via GDExtension are available directly:

```gdscript
# Planetary Hours
var calculator = PlanetaryHourCalculator.new()
var info = calculator.calculate_planetary_hour(lat, lon, Time.get_unix_time_from_system())
print(info["planet_name"])        # e.g. "Venus"
print(info["minutes_remaining"])  # Minutes until next planetary hour

# Celestial Mechanics
var orbits = PlanetaryOrbitCalculator.new()
var pos = orbits.get_planet_position(planet_index, julian_date)
# Returns geocentric ecliptic longitude, latitude, distance

# Inventory & Items
var inv = Inventory.new()
var item = HarvestItem.new()
item.plant_name = "Chamomile"
item.quality = HarvestItem.QUALITY_PRISTINE
item.stage = HarvestItem.STAGE_FRESH
inv.add_harvest_item(item)

# Plant Database
var db = PlantDatabase.new()
var plant = db.get_plant("Chamomile")
print(plant.planetary_ruler)  # Planet index
print(plant.element)          # Classical element
print(plant.properties)       # Culpeper's documented properties
```

---

## 🌱 Design Philosophy

Flora Philosophica presents itself as a cozy fantasy apothecary simulator, but the processes it teaches are technically accurate representations of **spagyric** preparation — the alchemical art of plant medicine. No recipe locks, no tutorial gates, no premium speed-ups. Real-world knowledge is rewarded with immediate mechanical advantage.

The primary reference is **Nicholas Culpeper's *Complete Herbal*** (1652). Each in-game plant maps to Culpeper's documented planetary ruler, classical element, and medicinal properties. The planetary hour system ensures that timing matters — just as it did for the historical herbalists who gathered simples at the hour of their ruling planet.

Design documents live in the `Flora_Philosophica/` Obsidian vault, including the master GDD v0.2, technical roadmap, location designs, and faction lore for **The Ternary Alchemical Order**.

---

## 🗺️ Roadmap Blueprint & Next Steps

This blueprint outlines the immediate next tasks required to build out the core game flow, UI, and character systems before expanding deeper into the laboratory and combat.

### 1. Inventory Screen UI
- [ ] Build a robust graphical inventory UI connecting to the 46-slot C++ `Inventory` backend.
- [ ] Support drag-and-drop or tap-to-select for the 36 grid slots and the 10 hotbar slots.
- [ ] Display item quality, stage, and Culpeper botanical properties on selection.

### 2. Main Menu & Settings
- [x] Fully hook up the Settings screen on the Main Menu.
- [x] Include volume sliders, display toggles, and UI scaling.
- [x] Create the overarching state flow from Main Menu -> Settings -> Character Selection -> Gameplay.

### 3. Character Creation (Pre-Gameplay)
- [x] Build the Character Selection screen.
- [x] **Character / Gender Options:** Choose between Basil (Masculine), Lavender (Feminine), and Sage (Non-Binary). The unchosen characters will automatically populate the world as NPCs.
- [x] **Location Override:** Allow the player to input their location for the astrological engine (noting explicitly that this is strictly local, replaces the hardcoded Medford default, and is never saved externally).
- [x] **Birth Data:** Input for date/time/place of birth to generate the player's Natal Chart (future integration with stats/lore).

### 4. Calendar UI
- [x] Add an interactive Calendar interface accessible from the HUD.
- [x] Track planetary days (Sunday = Sun, Monday = Moon, etc.).
- [x] Provide a visual way for players to plan long-term laboratory operations around celestial alignments.

### 5. Planetary Hour Schedule Screen
- [x] Build a dedicated UI screen (perhaps an expanded view of the current `clock_hud`) showing the complete Planetary Hour schedule for the current day.
- [x] Display exact start/end times for all 12 daylight and 12 nighttime hours based on the player's local sunrise/sunset calculations.
