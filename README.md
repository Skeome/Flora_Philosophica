# 🌿 Flora Philosophica

A cozy alchemical apothecary sanctuary game — and a stealth-educational guide to real-world **spagyric** plant alchemy. Forage, cultivate, and craft herbal preparations guided by a real-time astrological engine. A player who follows the in-game processes can actually produce real herbal spagyric preparations in their own kitchen.

Built with **Godot 4.7** and a **C++17 GDExtension** backend. Inspired by Nicholas Culpeper's *Complete Herbal* (1652) and the Western tradition of astrological herbalism.

---

## ✨ Features

### Real-Time Planetary Hour Engine
A C++ astronomical engine calculates true planetary hours from GPS coordinates and system clock. Sunrise and sunset are derived from solar position algorithms (Julian date, equation of time, solar declination, hour angle), dividing day and night into 12 planetary hours each following the Chaldean order. Planetary hours directly influence crafting potency, harvest quality, and atmospheric effects.

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
100+ herbs sourced from Culpeper's *Complete Herbal*, each with historically accurate planetary ruler, classical element, and medicinal properties.

### Overworld Exploration
Pixel-art tile-based overworld with y-sorted depth rendering. WASD + click-to-move controls (tap-to-move on mobile). Scene transitions via door zones connect multiple areas.

### Persistence
Full save/load system tracking inventory (46 slots), placed apparatus across 4 rooms, player position, planted herbs and growth stages, and timestamps.

---

## 🏗️ Architecture

Core systems are written in **C++17** and compiled as a GDExtension shared library. Game logic and UI are in **GDScript**. The C++ layer registers **13 classes** into Godot's ClassDB.

```
Flora_Philosophica/
├── src/                            # C++17 GDExtension source
│   ├── core/
│   │   ├── planetary_hour_calculator   # Solar position & planetary hours
│   │   └── planetary_orbit_calculator  # VSOP87 geocentric coordinates
│   ├── world/
│   │   ├── inventory                   # 46-slot inventory (10 hotbar + 36 grid)
│   │   ├── item                        # ItemDefinition + ItemDB (19 station types)
│   │   ├── harvest_item                # 22 processing stages, 5 quality tiers
│   │   ├── placed_item                 # Apparatus logic, timed processing
│   │   └── room_manager               # 4 rooms: Exterior, Cabin, Loft, Garden
│   ├── alchemy/
│   │   ├── plant_data                  # PlantDatabase — 100+ Culpeper entries
│   │   ├── recipe                      # Spagyric pipeline definitions
│   │   └── station                     # Processing nodes with real-time tick
│   ├── register_types.cpp              # GDExtension class registration
│   └── entry.cpp                       # GDExtension entry point
│
├── godot/                          # Godot 4.7 project
│   ├── scenes/
│   │   ├── main_menu.tscn              # Title screen w/ VSOP87 planet orbits
│   │   ├── main.tscn                   # Overworld (Ground, Walls, Rooftops,
│   │   │                               #   Counter-tops, Player, HUD)
│   │   ├── player.tscn                 # CharacterBody2D w/ directional anims
│   │   ├── plant_node.tscn             # Harvestable plant (quality from alignment)
│   │   ├── clock_hud.tscn              # Planetary hour display (♄♃♂☉♀☿☽)
│   │   └── pause_menu.tscn            # Non-pausing overlay (real-time design)
│   ├── scripts/
│   │   ├── game_manager.gd             # Autoload: save/load, scene state
│   │   ├── player.gd                   # Movement, animation, input
│   │   ├── world.gd                    # Door transitions, auto-save
│   │   ├── plant_node.gd               # Growth stages, astrological quality
│   │   ├── recipe_db.gd                # 9-step spagyric pipeline
│   │   ├── clock_hud.gd                # Planetary hour HUD w/ planet glyphs
│   │   ├── main_menu.gd                # Menu logic, panels, transitions
│   │   ├── pause_menu.gd               # Pause/resume, save, quit
│   │   └── planet_orbit_animator.gd    # VSOP87 orbit visualization (377 lines)
│   ├── assets/
│   │   ├── sprites/                    # Character, items, NPCs, plants,
│   │   │                               #   stations, tiles, world
│   │   ├── fonts/Archemy.otf           # Custom alchemical Unicode font
│   │   └── ui/                         # Menu art, planet PNGs, theme
│   └── bin/                            # Compiled .so + .gdextension manifest
│
├── Flora_Philosophica/             # Obsidian vault — design documents
│   ├── Flora_Philosophica_GDD.md       # Master GDD v0.2 (36.6 KB)
│   ├── Cpp_GD_Roadmap.md              # Technical architecture roadmap
│   ├── The Cabin/                      # Location design (Cabin, Lab, Storefront)
│   ├── The Ternary Alchemical Order/   # Faction lore
│   └── Flora Philosophica.canvas       # Obsidian design board
│
├── tests/                          # Test scaffolding (mirrors src/ structure)
├── scripts/                        # Build helpers
│   ├── build_android.sh                # Android build
│   └── build_web.sh                    # WebAssembly build
├── Culpepers_Complete_Herbal.epub  # Primary botanical reference source
├── CMakeLists.txt                  # CMake + CPM (godot-cpp, nlohmann_json)
└── save.json                       # Player save data
```

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
inv.add_item(item)
```

---

## 🌱 Design Philosophy

Flora Philosophica presents itself as a cozy fantasy apothecary simulator, but the processes it teaches are technically accurate representations of **spagyric** preparation — the alchemical art of plant medicine. No recipe locks, no tutorial gates, no premium speed-ups. Real-world knowledge is rewarded with immediate mechanical advantage.

The primary reference is **Nicholas Culpeper's *Complete Herbal*** (1652). Each in-game plant maps to Culpeper's documented planetary ruler, classical element, and medicinal properties. The planetary hour system ensures that timing matters — just as it did for the historical herbalists who gathered simples at the hour of their ruling planet.

Design documents live in the `Flora_Philosophica/` Obsidian vault, including the master GDD, technical roadmap, location designs, and faction lore for **The Ternary Alchemical Order**.
