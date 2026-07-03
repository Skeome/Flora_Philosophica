# 🌿 Flora Philosophica

A cozy alchemical apothecary sanctuary game — and a stealth-educational guide to real-world **spagyric** plant alchemy. Forage, cultivate, and craft herbal preparations guided by a real-time astrological engine. A player who follows the in-game processes can actually produce real herbal spagyric preparations in their own kitchen.

Built with **Godot 4.7** (HD-2D renderer — Octopath Traveller-style 3D world with Y-axis billboarded pixel-art sprites) and a **C++17 GDExtension** backend. Inspired by Nicholas Culpeper's *Complete Herbal* (1652), Robert Allen Bartlett's *The Temper of Herbs*, and the Western tradition of astrological herbalism.

---

## ✨ Features

### Real-Time Planetary Hour Engine
A C++ astronomical engine calculates true planetary hours from GPS coordinates and system clock. Sunrise and sunset are derived from solar position algorithms (Julian date, equation of time, solar declination, hour angle), dividing day and night into 12 planetary hours each following the Chaldean order (Saturn → Jupiter → Mars → Sun → Venus → Mercury → Moon). Planetary hours directly influence crafting potency, harvest quality, and atmospheric effects.

Default observer location: Medford, Oregon (42.3265°N, 122.8756°W) — overridden by GPS on mobile.

### VSOP87 Celestial Mechanics
A clean-room implementation of the VSOP87 planetary theory provides true geocentric ecliptic coordinates for all 7 classical planets. This powers the main menu's "dance of the spheres" — an animated orrery with real retrograde loops, where Venus traces her 8-year pentagram and Saturn completes his 29.45-year circuit.

### The Spagyric Pipeline
A historically accurate 9-step alchemical process — the core gameplay loop:

1. **Fresh → Dried** — Drying Rack (12 real hours)
2. **Dried → Ground** — Mortar & Pestle (1 real hour base, sped up via active session grinding)
3. **Ground → Tincture + Spent Residue** — Maceration Jar (7 real days)
4. **Residue → Calx Black** — Furnace (2.5 real hours target-curve calcination)
5. **Calx Black → Powdered Calx Black** — Mortar & Pestle (1 real hour)
6. **Powdered Calx Black → Calx Grey** — Leaching Dish (1 real hour)
7. **Calx Grey → Calx White** — Furnace (2.5 real hours target-curve calcination)
8. **Calx White → Salt Standard** — Furnace (2.5 real hours target-curve calcination)
9. **Salt Standard → Purified Salt** — Leaching Dish (1 real hour)

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

### Humoral Temperament Engine
Each plant carries four **humoral values** — Choler (Fire), Sanguine (Air), Phlegm (Water), and Melancholy (Earth) — stored as integer subdivisions (Jabir's 0–28 scale) on the C++ `PlantData` class. These values drive **Al-Kindi's geometric temper calculation**: Walter of Odington's intensity ratios (2^(n/7)) resolve each humor into parts of Hot, Cold, Wet, and Dry, then logarithmic ratios yield the classical degree (1st through 4th) along each axis. The result is the plant's true **temperament** — e.g. "Hot in the 2nd degree, Dry in the 1st degree" — matching the historical pharmacopoeia.

The same calculation engine supports **compound medicines**: when multiple simples are combined, their individual humoral parts are summed and re-evaluated through Al-Kindi's ratio mathematics (as described by Ibn Biklarish and Arnald de Villanova), producing the compound's net temperament and degree.

### Natal Chart & Astrological Aspects
At character creation, the player enters their date, time, and place of birth. The VSOP87 engine calculates true geocentric ecliptic longitudes for all 7 classical planets at the birth moment, placing each planet in its zodiac sign. Zodiac placements grant **flat +2 stat bonuses** to the sign's ruling planet.

Planetary aspects are computed from angular separations with traditional orbs:

| Aspect | Angle | Orb | Effect |
|---|---|---|---|
| **Conjunction** | 0° | 8° | Synergy: +5% effectiveness to both planets' actions |
| **Sextile** | 60° | 6° | Harmony: +10% efficiency to both planets' actions |
| **Square** | 90° | 8° | Tension: +15% potency but +10% mental fatigue cost |
| **Trine** | 120° | 8° | Harmony: +10% efficiency to both planets' actions |
| **Opposition** | 180° | 8° | Tension: +15% potency but +10% mental fatigue cost |

Aspects generate **passive traits** that persist throughout gameplay, influencing crafting, combat, and harvest outcomes.

### Jabir's "Analysis by Fire" Data Sheet
Accessible from the Pause Menu's **Analysis by Fire** tab, this interactive data sheet implements the classical fire analysis protocol from Bartlett's *The Temper of Herbs*. Select any herb from the database dropdown to view:

- **Alchemical Separation of Elements**: A 4-row breakdown showing each fraction (Phlegm/Water, White Spirit/Air, Red Spirit/Fire, Residue/Earth) with weight percentages and dominant/secondary classical qualities
- **Derived Humoral Balance**: The calculated Hot/Cold and Wet/Dry degree using Al-Kindi's geometric scale
- **Primary Quality Parts**: Raw geometric parts for Hot, Cold, Wet, and Dry
- **Jabir Subdivision Values**: The humoral values mapped back to Jabir's 28-point scale

### Plant Database
100+ herbs sourced from Culpeper's *Complete Herbal*, each with historically accurate planetary ruler, classical element, medicinal properties, and four-humor temperament values (Choler, Sanguine, Phlegm, Melancholy). The C++ `PlantDatabase` class alone spans 68 KB.

### Apparatus & Stations
19 apparatus types available for placement across 4 rooms (Exterior, Cabin Main, Cabin Loft, Garden): Fireplace, Drying Rack, Mortar & Pestle, Maceration Jar, Compost Bin, Work Bench, Copper Alembic, Glass Flask, Glassblowing Station, Distillation Train, Soxhlet Extractor, Pelican Flask, Retort Train, Terrarium, Bookshelf, Storage Chest, Mailbox Post, Furnace, and Leaching Dish. Each has tile dimensions, tier requirements, and indoor/outdoor placement flags.

### Overworld Exploration (HD-2D)
Octopath Traveller-style HD-2D overworld: 3D terrain with real depth, DirectionalLight3D shadows, and Y-axis billboarded pixel-art sprites. Fixed diorama camera (38° FOV, 28° downtilt) with smooth player tracking, depth-of-field tilt-shift blur, bloom, and ACES filmic tonemapping. Planetary-hour-driven lighting rig tints the world's directional light, ambient, and fog in real time. Walk freely with **WASD** or **click-to-move** (raycast to XZ ground plane). Scene transitions via Area3D door zones connect multiple areas. The player character, "Basil," uses a CharacterBody3D with 4-directional AnimatedSprite3D animations. Plant sprites are Y-axis billboarded and fully shaded.

### Combat & Meditation
Physical combat does not exist in the overworld. Combat is initiated via **Meditation ("Guided Imagery")** at specific nodes, entering the "Spirit World." Here, players fight spiritual essences of monsters and inner shadow clones (akin to Dark Link). Combat relies on drawing sigils via touch input, with the current Planetary Hour providing massive damage multipliers (up to 4x for matching Day & Hour). Defeat results in "Mental Fatigue," lowering laboratory crafting success. Active natal aspect traits (Synergy, Harmony, Tension) modify damage and fatigue costs during combat.

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

### Real-Time 1:1 Simulation & Autonomous Stations
The pause menu intentionally does **not** pause the SceneTree. In fact, all alchemical operations occur in **strict 1:1 real-time**. Maceration takes 7 actual days. 

Apparatus stations are highly autonomous and can be configured:
- **Strict Mode**: Only process during a specific astrological window (e.g., Sun hour for a Sun plant).
- **Synergistic Mode**: Process when any harmonious celestial influence is active.
- **Continuous Mode**: Run 24/7, accruing both positive and negative astrological quality modifiers over time.

Players can "drop in" to active stations (like the Mortar or Furnace) for **session-based mini-games**. Engaging during aligned planetary hours injects massive quality bonuses into the `accumulated_quality` pool and applies time compression (e.g. 1 minute of active grinding speeds up the Mortar's 1-hour timer by 5 minutes). The Furnace requires following a delicate target temperature ramp (4°C/minute) without vitrifying the batch.

---

## 🏗️ Architecture

Core systems are written in **C++17** and compiled as a GDExtension shared library. Game logic, natal chart computation, Al-Kindi temper calculations, and UI are in **GDScript**. The C++ layer registers **13 classes** into Godot's ClassDB:

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
│   │   │                               #   + 4-humor temper values (choler/sanguine/phlegm/melancholy)
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
│   │   ├── main.tscn                   # Legacy 2D overworld (deprecated)
│   │   ├── overworld_3d.tscn           # HD-2D overworld (3D terrain + billboarded sprites)
│   │   ├── cabin_main.tscn             # Cabin interior (Stations, Player, HUD,
│   │   │                               #   InventoryUI, FloorLayer)
│   │   ├── player.tscn                 # "Basil" — CharacterBody2D, 4-dir anims (legacy)
│   │   ├── player_3d.tscn              # "Basil" — CharacterBody3D, HD-2D billboarded
│   │   ├── plant_node_3d.tscn          # HD-2D harvestable plant (Area3D, billboarded)
│   │   ├── plant_node.tscn             # Harvestable plant (quality from alignment)
│   │   ├── inventory_ui.tscn           # Hotbar + inventory panel (per-scene instance)
│   │   ├── clock_hud.tscn              # Planetary hour display (♄♃♂☉♀☿☽)
│   │   └── pause_menu.tscn             # Non-pausing overlay (real-time design)
│   ├── scripts/
│   │   ├── game_manager.gd             # Autoload: save/load, C++ object refs,
│   │   │                               #   natal chart, aspects, Al-Kindi temper calc
│   │   ├── player.gd                   # Movement, animation, input (legacy 2D)
│   │   ├── player_3d.gd                # HD-2D CharacterBody3D player
│   │   ├── overworld_3d.gd             # HD-2D overworld root (door transitions, plants)
│   │   ├── plant_node_3d.gd            # HD-2D plant node (Area3D harvesting)
│   │   ├── billboard_sprite.gd         # 4-directional sprite facing from camera angle
│   │   ├── world.gd                    # Door transitions, auto-save (legacy 2D)
│   │   ├── plant_node.gd               # Growth stages, astrological quality
│   │   ├── inventory_ui.gd             # Hotbar (10 slots) + grid panel (36 slots),
│   │   │                               #   drag-drop, info box, _draw() rendering
│   │   ├── recipe_db.gd                # 9-step spagyric pipeline
│   │   ├── clock_hud.gd                # Planetary hour HUD w/ planet glyphs
│   │   ├── main_menu.gd                # Menu logic, TTAO panel, credits, links
│   │   ├── pause_menu.gd              # Pause/resume, save, natal chart tab,
│   │   │                               #   Analysis by Fire data sheet tab
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
├── BartlettTemperofHerbs.txt       # OCR text — R.A. Bartlett, The Temper of Herbs
├── BartlettTemperofHerbs.pdf       # Source PDF
├── TemperofHerbsImages/            # Scanned pages (142 images) — tables, diagrams
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
print(plant.choler)           # Humoral value: Fire (0-28)
print(plant.sanguine)         # Humoral value: Air (0-28)
print(plant.phlegm)           # Humoral value: Water (0-28)
print(plant.melancholy)       # Humoral value: Earth (0-28)

# Al-Kindi Temper Calculation
var temper = GameManager.calculate_spagyric_temper(
    plant.choler, plant.sanguine, plant.phlegm, plant.melancholy
)
print(temper["hot_cold_degree"])  # Positive = Hot, Negative = Cold
print(temper["wet_dry_degree"])   # Positive = Wet, Negative = Dry
print(temper["hot_parts"])        # Geometric parts of Hot
print(temper["cold_parts"])       # Geometric parts of Cold

# Natal Chart (computed from birth data on load)
print(GameManager.sign_placements)  # {"Sun": "Leo", "Moon": "Cancer", ...}
print(GameManager.active_aspects)   # [{aspect: "Trine", planet1: "Venus", ...}]
print(GameManager.player_traits)    # [{name: "Venus Trine Jupiter", ...}]
```

---

## 🌱 Design Philosophy

Flora Philosophica presents itself as a cozy fantasy apothecary simulator, but the processes it teaches are technically accurate representations of **spagyric** preparation — the alchemical art of plant medicine. No recipe locks, no tutorial gates, no premium speed-ups. Real-world knowledge is rewarded with immediate mechanical advantage.

The primary references are **Nicholas Culpeper's *Complete Herbal*** (1652) and **Robert Allen Bartlett's *The Temper of Herbs***. Each in-game plant maps to Culpeper's documented planetary ruler, classical element, and medicinal properties, while Bartlett's work provides the mathematical framework for humoral temperament — Jabir ibn Hayyan's elemental subdivisions, Al-Kindi's geometric ratio scale, and Walter of Odington's intensity progressions. The planetary hour system ensures that timing matters — just as it did for the historical herbalists who gathered simples at the hour of their ruling planet.

Design documents live in the `Flora_Philosophica/` Obsidian vault, including the master GDD v0.2, technical roadmap, location designs, and faction lore for **The Ternary Alchemical Order**.

---

## 🗺️ Roadmap Blueprint & Next Steps

This blueprint outlines the immediate next tasks required to build out the core game flow, UI, and character systems before expanding deeper into the laboratory and combat.

### 1. Inventory Screen UI
- [x] Build a robust graphical inventory UI connecting to the 46-slot C++ `Inventory` backend.
- [x] Support drag-and-drop or tap-to-select for the 36 grid slots and the 10 hotbar slots.
- [x] Display item quality, stage, Culpeper botanical properties, and humoral temperament on selection.
- [x] Instance `InventoryUI` per-scene (in `main.tscn` and `cabin_main.tscn` only) to prevent the hotbar from drawing over the main menu and other non-gameplay screens.

### 2. Main Menu & Settings
- [x] Fully hook up the Settings screen on the Main Menu.
- [x] Include volume sliders, display toggles, and UI scaling.
- [x] Create the overarching state flow from Main Menu -> Settings -> Character Selection -> Gameplay.

### 3. Character Creation (Pre-Gameplay)
- [x] Build the Character Selection screen.
- [x] **Character / Gender Options:** Choose between Basil (Masculine), Lavender (Feminine), and Sage (Non-Binary). The unchosen characters will automatically populate the world as NPCs.
- [x] **Location Override:** Allow the player to input their location for the astrological engine (noting explicitly that this is strictly local, replaces the hardcoded Medford default, and is never saved externally).
- [x] **Birth Data:** Input for date/time/place of birth to generate the player's Natal Chart with stat bonuses and aspect traits.

### 4. Calendar UI
- [x] Add an interactive Calendar interface accessible from the HUD.
- [x] Track planetary days (Sunday = Sun, Monday = Moon, etc.).
- [x] Provide a visual way for players to plan long-term laboratory operations around celestial alignments.

### 5. Planetary Hour Schedule Screen
- [x] Build a dedicated UI screen (perhaps an expanded view of the current `clock_hud`) showing the complete Planetary Hour schedule for the current day.
- [x] Display exact start/end times for all 12 daylight and 12 nighttime hours based on the player's local sunrise/sunset calculations.

### 6. Humoral Temperament & Analysis by Fire
- [x] Add 4-humor values (choler, sanguine, phlegm, melancholy) to C++ `PlantData` class.
- [x] Implement Al-Kindi's geometric temper calculation (`calculate_spagyric_temper()`) in GDScript.
- [x] Build "Analysis by Fire" data sheet tab in the Pause Menu with plant selection dropdown.
- [x] Display elemental separation fractions, weight percentages, and calculated degrees.
- [ ] Extend compound medicine UI for multi-herb recipes (sum humoral parts, re-derive temperament).
- [ ] Visual lab minigame for fire analysis procedure (heating stages, fraction collection).

### 7. Natal Chart & Astrological Aspects
- [x] Compute natal chart from birth data using VSOP87 geocentric positions.
- [x] Zodiac sign placements with flat stat bonuses to ruling planet.
- [x] Aspect detection (Conjunction, Sextile, Square, Trine, Opposition) with orb calculations.
- [x] Passive trait generation from active aspects (Synergy, Harmony, Tension effects).
- [x] Build "Natal Chart" tab in Pause Menu displaying stats, placements, and traits.
- [ ] Apply aspect trait modifiers to crafting, combat, and harvest systems.

### 8. Real-Time Autonomous Stations & Distillation
- [x] Refactor C++ `PlacedItem` to support 1:1 real-time durations (Maceration = 7 days) and `AutonomousMode` (Strict, Synergistic, Continuous).
- [x] Implement session-based Mortar & Pestle minigame (time compression).
- [x] Implement 20°C–600°C target-curve Furnace calcination minigame.
- [ ] Implement Distillation Train (7-cycle spirit purification with drop counting).

### 9. HD-2D Visual Pivot
- [x] Prototype HD-2D lighting concept (planetary-hour-driven DirectionalLight3D + Environment).
- [x] Build CharacterBody3D player with AnimatedSprite3D (4-directional, Y-axis billboard).
- [x] Create HD-2D overworld scene with 3D terrain, diorama camera, and door transitions.
- [x] Convert PlantNode to Area3D with Sprite3D billboard and Label3D.
- [x] Add BillboardSprite utility for camera-relative 4-directional facing.
- [ ] Convert cabin interior to HD-2D (3D floor/walls, Area3D doors, StationNode3D).
- [ ] Replace placeholder meshes with textured 3D terrain and props.
- [ ] Port existing Basil sprite sheets to AnimatedSprite3D SpriteFrames.
- [ ] Add particle effects (fireflies, pollen, fog wisps) tied to planetary hours.
- [ ] NPC sprites with BillboardSprite 4-directional facing.
