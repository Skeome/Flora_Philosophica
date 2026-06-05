# Flora Philosophia
## Game Design Document — Version 0.2 — June 2026

*The Ternary Alchemical Order · Solve Et Coagula · Mercurius · Sulphur · Sal · Operative Spagyrica*

☉ ☽ ♂ ☿ ♃ ♀ ♄

---

## Table of Contents

1. [Core Concept](#1-core-concept)
2. [Visual Identity & Atmosphere](#2-visual-identity--atmosphere)
3. [The Real-Time Astrological Engine](#3-the-real-time-astrological-engine)
4. [The Laboratory](#4-the-laboratory)
5. [The Residue Purification Pipeline](#5-the-residue-purification-pipeline)
6. [Product Progression Ladder](#6-product-progression-ladder)
7. [Character Progression: The Seven Oblations](#7-character-progression-the-seven-oblations)
8. [Exploration & Combat](#8-exploration--combat)
9. [Idle Mechanics & Passive Generation](#9-idle-mechanics--passive-generation)
10. [Economy & Customer System](#10-economy--customer-system)
11. [The Ternary Alchemical Order](#11-the-ternary-alchemical-order)
12. [Equipment & Progression Tiers](#12-equipment--progression-tiers)
13. [Technical Architecture](#13-technical-architecture)
- [Appendix A — Source Fidelity Note](#appendix-a--source-fidelity-note)
- [Open Design Questions](#open-design-questions)

---

## 1. Core Concept

Flora Philosophia is a mobile RPG that functions as a stealth-educational tool for real-world plant alchemy. What presents itself as a cozy fantasy apothecary simulator is, in practice, a technically accurate guide to operative spagyrics. A player who follows the in-game processes — maceration, calcination of the residue, leaching and precipitation of salts, and recombination — will successfully produce real herbal spagyric preparations.

The game does not condescend to the player. There are no recipe locks, no tutorial pop-ups demanding completion before progression, and no premium speed-ups. Real-world knowledge is rewarded with immediate mechanical advantage.

### 1.1 Title

- Working title: **Flora Philosophia**
- Considered alternatives: *Alchemilla's Hearth*, *Solve & Sprout*, *The Hermetic Herbalist*
- **Spagyricus** and **Spagyria** are reserved — both are chosen *nomina* of practising spagyrists (Robert Allen Bartlett and John H. Reid III respectively) and must not be used

### 1.2 Setting

- A secluded mountain cabin that evolves from a humble forager's hut into a master Alchemist's sanctuary
- The cabin is an explorable space — the player physically walks their character between laboratory stations, not through static menus
- As equipment is purchased and placed, the cabin visually fills and transforms, providing a tangible sense of progression

### 1.3 The Hook

> *"What appears to be a casual fantasy apothecary simulator is actually a stealth-educational tool. If a player follows the game's recipes in real life, they will successfully create real herbal spagyrics."*

The game achieves this not through text explanations or tutorials but through the structure of its mechanics. The compost bin, the calcination failure states, the naming conventions of the residue pipeline, and the astrological timing system all teach operative spagyrics through play.

### 1.4 Player & Party

- The player controls a single Alchemist protagonist with a singular storyline
- Recruitable NPCs are planned as a future addition — companions met through questlines who can assist in exploration, combat, or laboratory work
- Open-world multiplayer is under long-term consideration; the single-player loop will be fully designed and stable before any multiplayer scope is added

---

## 2. Visual Identity & Atmosphere

### 2.1 Perspective & Controls

- **3/4 top-down perspective** — the spatial freedom of *Chrono Trigger* rendered in the hand-painted aesthetic of *Another Eden* (or *Sword × Staff*). This is essentially a Stardew Valley clone
- The world is a 2D plane with depth (X and Y movement), built from high-end layered hand-painted assets
- **Tap-to-Move:** Player taps a tile or area; the character pathfinds automatically around obstacles
- **Virtual Joystick:** Floats wherever the player holds their finger on the screen; enables precise manual movement and enemy dodging
- Players can walk behind trees, cross stone bridges, and discover hidden clearings off the main path

### 2.2 Atmospheric Overlays

The world's lighting and weather dynamically reflect the real-time Planetary Hour, calculated from the player's device clock and geographic location.

| Planetary Hour | Visual Atmosphere |
|---|---|
| **Saturn ♄** | Shadows lengthen; palette shifts to muted leaden tones; heavy ambient track |
| **Sun ☉** | Warm golden light floods the environment; birdsong increases |
| **Venus ♀** | Warm copper-green glow; flowers visibly bloom or release shimmering pollen |
| **Mars ♂** | Iron-red energy pulses from Mars-ruled plant nodes; enemies gain visual intensity |
| **Moon ☽** | Silvery mist settles; lunar plants glow faintly; night biomes become accessible |
| **Mercury ☿** | Quicksilver shimmer on water surfaces; fast-moving environmental particles |
| **Jupiter ♃** | Deep indigo sky tint; rare plant nodes have increased spawn chance |

### 2.3 Botanical Illustrations

- Harvesting a plant triggers a full-screen pop-up styled as a historical grimoire page, influenced by Culpeper's *Complete Herbal*
- Each illustration details the plant's planetary ruler, elemental associations, and Culpeper's documented medicinal properties
- These illustrations populate the in-game **Plant Compendium** as discoveries are made

### 2.5 UI Design Direction

The HUD and inventory UI will follow a *Stardew Valley*-inspired layout as development matures:

- **Hotbar** along the bottom edge of the screen — quick access to harvested items, tools, and active preparations
- **Item slots** use simple icon + quantity format; quality tier indicated by a colored border (gold = Pristine, white = Standard, grey = Debased)
- **Inventory panel** opens as an overlay from the hotbar, not a separate screen
- **Station interaction** opens a contextual panel near the station rather than a full-screen menu, preserving spatial awareness of the room
- The astrological clock HUD (top-left) and room indicator are persistent; all other UI is contextual
- Current placeholder UI (colored rectangles, text panels) will be replaced once the core mechanics are stable

### 2.6 Audio

- Tracker-styled compositions or classic hardware synthesis — a blend of nostalgic sample-based tracker music with modern atmospheric sound design
- Ambient audio layers shift with Planetary Hours, reinforcing the real-world timing system through sound as well as visuals
- Laboratory sounds are grounded: glass clinks, furnace roar, the hiss of a distillation train, the crystalline sound of salt precipitating

---

## 3. The Real-Time Astrological Engine

Flora Philosophia synchronizes its internal clock entirely to the player's real world. There is no accelerated game calendar. This is the game's most significant design decision — it transforms the app from an entertainment product into a functional lifestyle companion for the operative spagyrist.

### 3.1 Planetary Hour Calculation

- The engine uses the player's device GPS and system clock to calculate actual local sunrise and sunset for their specific latitude and longitude
- **Unequal Hours** are used throughout: the daylight period is divided into 12 equal segments and the night period into 12 equal segments independently
- Because of this, a planetary hour might last 75 real minutes in midsummer and shrink to 45 minutes in midwinter — teaching the player how seasons shift celestial timing as a natural consequence of engagement
- The **Chaldean order** of planetary hours is followed: Saturn → Jupiter → Mars → Sun → Venus → Mercury → Moon, repeating from sunrise

### 3.2 Day and Hour Correspondence

Each day of the week carries a ruling planet. Each day contains at minimum one window where the current planetary hour matches the ruling planet of that day — and usually more.

| Day | Ruling Planet |
|---|---|
| Sunday | ☉ Sun |
| Monday | ☽ Moon |
| Tuesday | ♂ Mars |
| Wednesday | ☿ Mercury |
| Thursday | ♃ Jupiter |
| Friday | ♀ Venus |
| Saturday | ♄ Saturn |

> *Design note: There are no truly locked-out planetary hours. Every planet receives at minimum one window per day. If a player misses the sunrise Mars hour on Tuesday, they simply wait for the next Mars window. This is not a restriction — it is an accurate reflection of how operative alchemists schedule their work.*

### 3.3 Harvest Quality

- Harvesting a plant during its matching planetary **day and hour** yields **Pristine Celestial Harvest** quality
- Harvesting at any other time produces **Standard** quality
- Harvesting at the opposite planetary hour (e.g. a Solar plant during a Saturnian night hour) produces a **Stressed / Debased** specimen, reducing final product potency and increasing the probability of laboratory failures
- No mechanic blocks access to any herb at any time — quality is the only modifier

### 3.4 Consecration

- A preparation can be combined at any time after its processing window completes — but combining during the matching planetary day and hour stamps it as **Astro-Synchronised**
- Astro-Synchronised products command a significant price premium from elite customers and provide enhanced stat bonuses when consumed via the Seven Oblations
- Unsynchronised products are still valid and functional — this system rewards the disciplined player without punishing the casual one

---

## 4. The Laboratory

The laboratory is not a menu. It is an explorable room within the mountain cabin. The player physically walks their character to each station to initiate actions, check progress, and collect results. As better equipment is purchased, it is placed in the lab space, which visually expands and fills across the four equipment tiers.

### 4.1 Real-Time Processing

- **1 real hour ≈ 2 in-game weeks**
- Approximately 30 real minutes per in-game week; ~4 real minutes per in-game day
- **There are no premium speed-ups.** This is non-negotiable. The pacing exists to respect the operative nature of the work.
- Players manage multiple preparations simultaneously — different projects at different stages — ensuring there is always something to act on during short play sessions

### 4.2 Sandbox Crafting

The laboratory operates as a true sandbox. There are no recipe locks graying out options. Any item can be placed into any apparatus. The game engine recognises the interaction and determines the result based on real chemical and alchemical logic.

- A player with prior knowledge of spagyrics can begin producing Spagyric Tinctures on Day 1 by ignoring the compost bin and calcining their first Residue immediately
- This sequence-breaking is rewarded with an immediate economic and progression advantage
- Invalid processing is still possible — burning something incorrectly yields **Generic Ash** rather than a named Salt — but the game does not prevent the attempt

### 4.3 The Compost Bin Trap

The compost bin is positioned outside the cabin and presented early as a natural destination for Spent Plant material. The game subtly encourages its use — it generates basic fertilizer for a small garden plot.

- Once any material touches the compost, it is **irretrievable** — effectively a thematic trash system
- There is no visible trash can, no explicit "delete item" button. The compost is the only disposal route for most players in the early game
- The revelation arrives via the Ternary Order letter system: *"The spent plant is not waste, marry it with fire and water until it is pure."*
- Players who have been faithfully composting experience the intended **"Aha!" moment** — recognizing that they have been discarding the Salt principle, the physical Body of the plant, for hours

### 4.4 Failure States

The laboratory enforces physics-based consequences. There is no generic "Craft Failed" screen.

| Failure Type | Consequence |
|---|---|
| Minor heat excess | Visual warning: glass rattles audibly, liquid boils aggressively. Player has time to reduce heat. |
| Significant heat excess | No warning. Operation destroyed. Batch must be restarted. |
| Sealed flask with insufficient vapour room | Setup explodes, littering the lab floor with Broken Glass. |
| Invalid calcination | Still burns if flammable. Yields Generic Ash. The sandbox permits it; chemistry determines the result. |

### 4.5 The Glassblowing Station

- An optional purchasable station for the sanctuary, available from Tier 2 onward
- Glassblowing is an **independent skill** with its own experience track, separate from laboratory tier and character stats
- The player levels Glassblowing exclusively by practising it — blowing vessels, repairing broken glass, and attempting progressively complex shapes
- Glassblowing is a **mini-game**: the player controls breath pressure and rotation using on-screen input, shaping molten glass into the target vessel form. Accuracy determines the quality and tolerance rating of the finished piece.
- Allows the player to collect Broken Glass from explosions (or gather Sand from map biomes) and melt it down to hand-blow new vessels
- Higher Glassblowing skill unlocks more complex vessel types (retorts, pelicans, condensers) and produces pieces with higher heat and pressure tolerances than purchased glassware
- A skilled glassblower can produce equipment that is meaningfully superior to anything available from vendors — rewarding investment in the skill over time

---

## 5. The Residue Purification Pipeline

The Tria Prima — Mercurius (Spirit), Sulphur (Soul), and Sal (Body) — are the educational core of Flora Philosophia. The residue pipeline teaches the player to process the physical Body of the plant rather than discard it, completing the spagyric triad.

Item names update dynamically as the material changes state. The player's inventory is itself a progress tracker.

| Stage | Item Name | Process | Notes |
|---|---|---|---|
| 1 | **Spent [Plant] / Residue** | Fibrous matter remaining after liquid extraction. Dried and ready for the furnace. | Many players discard to the compost bin here — this is the "trap." |
| 2 | **Caput Mortuum** | Residue scorched black in the crucible. First burn stage. | No warning if heat is poorly managed — vessel may crack. |
| 3 | **Calx of [Plant] (Ash)** | Continued calcination to white ash. Full burn achieved. | Colour shift to white is the visual cue of completion. |
| 4 | **Calx of [Plant] (Alkali)** | First leaching with distilled water and subsequent precipitation. | Repeat until precipitate is clear; water quality matters. |
| 5 | **Calx of [Plant] (Salt)** | Further leaching and precipitation to clarify matter. | Item name updates automatically in inventory. |
| 6 | **Salt of [Plant]** | Final calcination yields pure crystalline salt, permanent. | Ready to recombine with tincture for Spagyric completion. |

---

## 6. Product Progression Ladder

Players with no prior knowledge begin at the bottom. Players with existing operative knowledge may begin anywhere — the sandbox system does not gate access. Customer orders scale to match the player's demonstrated product level.

| Product | Method | Notes |
|---|---|---|
| **Basic Tincture** | Maceration of herb in alcohol. ~1 real hour. | Entry level. Most early customers request these. |
| **Spagyric Tincture** | Basic tincture + purified Salt of [Plant] recombined. | Requires completing the full residue purification pipeline. |
| **Elixirs & Magisteries** | Advanced distillation using retort apparatus. | Unlocks Alchemist and Noble customers. |
| **Ens Tincture** | Essence extracted using deliquesced salts (Sal Tartar). | High-value mail orders; rare and time-intensive. |
| **Plant Stone** | Ultimate masterwork via cohobation in a Pelican flask. | One per plant; permanent. Required for Seven Oblations completion. |

### 6.1 The Plant Stone

The Plant Stone is the endgame masterwork for each individual plant. It is achieved through **cohobation** — the cyclic process of continuous distillation and recombination using a Pelican flask, which feeds its own distillate back over the fixed base repeatedly.

- One Plant Stone per plant — each is a permanent, discrete item
- The Pelican flask becomes available at Tier 4 and is required for cohobation
- Completing a Plant Stone is the prerequisite for finalising a Seven Oblations entry for that plant's planetary ruler
- Plant Stones command the highest prices from elite customers and are requested by Wizards, Nobles, and Ternary Order-affiliated NPCs

---

## 7. Character Progression: The Seven Oblations

Flora Philosophia has no experience point bar. Character stats are increased exclusively through an internal alchemy system called the **Seven Oblations**. The player selects one plant for each of the Seven Planetary Rulers and refines that plant to its Plant Stone via the full spagyric pipeline, including cohobation. Consuming the masterwork product permanently increases the corresponding stat.

The plant chosen for each Oblation is **permanent** — it cannot be changed once work has begun. This forces a meaningful early-game decision and gives each player's character a unique botanical identity.

| Planetary Ruler | Associated Herbs (Culpeper) | RPG Stat | Harvest Notes |
|---|---|---|---|
| **☉ Sun** | Rosemary, Angelica, St. John's Wort | Vitality / Max HP | Solar herbs; harvest Sunday Solar hour |
| **☽ Moon** | Mugwort, Willow, Cleavers | Mana / MP | Monday; lunar hour near midnight |
| **♂ Mars** | Nettle, Basil, Garlic | Attack Power / Phys DMG | Tuesday; harvesting in iron-red glow |
| **♀ Venus** | Yarrow, Rose, Mint | Charisma / Shop Prices | Friday; copper-green ambient light |
| **☿ Mercury** | Lavender, Fennel, Caraway | Agility / Evasion | Wednesday; quicksilver shimmer |
| **♃ Jupiter** | Lemon Balm, Dandelion, Sage | Luck / Crit Chance | Thursday; indigo sky tint |
| **♄ Saturn** | Comfrey, Horsetail, Mullein | Defense / Armor | Saturday; leaden shadows lengthen |

> *Culpeper's planetary attributions are used throughout. These are historically documented and operatively recognised within the spagyric tradition. The list given in each planetary category is not exhaustive — players may choose any Culpeper-attributed plant for that ruler.*

---

## 8. Exploration & Combat

### 8.1 The Map

- The world is navigated via the Chrono Trigger-style 3/4 perspective with tap-to-move pathfinding and a virtual joystick
- Multiple biomes are accessible as the player progresses: local meadows, dense forest, craggy mountain paths, riverbanks, overgrown ruins
- Plant nodes are physical objects in the environment — a glowing patch of St. John's Wort nestled by a cliffside, Nettle clusters near iron-rich soil
- The player walks their character up to a node and taps to harvest; the Culpeper botanical illustration pop-up triggers on first discovery
- Biome availability and rare plant spawn rates are influenced by the current Planetary Hour

### 8.2 Visible Enemies

- No random encounters. All enemies are visible on the map and can be approached or avoided.
- The player can attempt to use the joystick to navigate around enemy patrols
- Walking into an enemy (or being caught) initiates combat **without a transition screen**
- The Chrono Trigger model is followed precisely: characters draw weapons and take combat stances on the map itself; the sigil-drawing UI fades in over the environment
- Enemies shift elemental weaknesses based on the current Planetary Hour — a wolf gains significant speed and evasion during a Mercury hour, for example

### 8.3 Sigil-Based Combat

There are no Attack / Magic menu buttons. To act in combat, the player manually draws alchemical or astrological sigils on the screen using touch input.

- The accuracy and speed of the drawing determine the power of the resulting strike or spell
- Drawing ♂ (Mars) launches a physical, fire-aligned strike using the energy of Mars-ruled material
- Drawing ☽ (Moon) releases a healing wave using Mugwort energy
- Successfully completing a sigil triggers a high-quality combat animation, contrasting the slow methodical nature of laboratory work
- Partially drawn or misdrawn sigils may produce weakened effects or misfire

### 8.4 Enemy Types & Drops

| Enemy | Notable Drops | Notes |
|---|---|---|
| Slimes | Gelatin, base matter | Common in early biomes. Weak to most sigils. |
| Wolves | Furs, fangs (armour crafting) | Gain speed and evasion during Mercury hours. |
| Elementals | Elemental dust (reaction catalysts) | Type-specific drops for laboratory use. |
| Wild Animals | Varied crafting materials | Bears, boars, deer; scale across biomes. |
| Monsters | Rare equipment components | Classic JRPG types — goblins, wraiths, constructs. |

---

## 9. Idle Mechanics & Passive Generation

Flora Philosophia does not use an away-grinding system for the player character. The idle layer is entirely contained within the Sanctuary (the mountain cabin and its grounds).

### 9.1 Enchanted Terrariums

- Passive resource generators constructed and placed within the sanctuary
- Each terrarium is configured for a specific plant and slowly grows and auto-harvests that plant over time, depositing cuttings into a collection hopper
- Growth rate is subtly influenced by real-world Planetary Hours — a Sun terrarium growing St. John's Wort grows faster during Solar hours
- Tending the terrarium during the correct planetary hour grants a quality bonus to the next harvest
- Terrariums unlock once the player has made at least one successful wild harvest of the target plant

### 9.2 The Idle Loop

The laboratory continues processing any active preparations whether the app is open or closed.

1. Player logs in, collects terrarium hoppers and any completed laboratory preparations
2. Starts new macerations, calcinations, or distillations — setting multiple parallel projects running
3. Spends active session time exploring the map, fighting enemies, fulfilling customer orders, or tending the lab
4. Logs off with several projects running simultaneously

> *Designed for short, satisfying daily sessions of 5–15 minutes of active engagement, with passive processing running continuously in the background.*

### 9.3 Notification System

- *"Your St. John's Wort tincture has finished macerating."*
- *"Your terrarium has produced 8 Mugwort cuttings — collect them before the hopper fills."*
- *"A Mars Hour begins in 10 minutes — your Nettle nodes are glowing."*
- *"A new letter has arrived at the cabin."*

---

## 10. Economy & Customer System

### 10.1 Customer Types

| Customer | Request Complexity | Reward |
|---|---|---|
| **Villagers** (early game) | Simple remedies: *"I burned my hand — do you have St. John's Wort?"* | Modest gold |
| **Adventurers' Guild** | Specific elemental materials and basic combat preparations | Moderate gold |
| **Nobles** | Elixirs and Magisteries | High gold; occasional rare equipment |
| **Visiting Alchemists** | Rare products; recognise quality Spagyric work | High gold; lore exchange |
| **Wizards** | Ens Tinctures and Plant Stones | Massive payouts |
| **Ternary Order** (mail only) | No gold — deliver cryptic lore hints sealed with the Order's wax seal | Lore progression |

### 10.3 Menstruum (Alcohol) Sourcing

The menstruum — the alcoholic solvent used for maceration and tincture preparation — can be obtained in two ways, reflecting short-term and long-term play strategies.

- **Purchase from vendors (short-term):** Basic spirits are available for purchase from town vendors. This allows new players to begin laboratory work immediately without an agriculture prerequisite. Vendor spirits are of Standard quality and carry no special properties.
- **Home fermentation and distillation (long-term):** Players can invest in Grape Vine or Wheat seeds, cultivate a small plot on the cabin grounds, ferment the harvest, and distill their own menstruum using in-lab apparatus. Home-distilled spirits are of higher quality, carry a celestial stamp if produced under the correct Planetary Hours, and cost only time and labour after the initial seed investment. This is the authentic operative path and is rewarded with better final product quality and customer pricing.

### 10.2 Revenue & Spending

- Gold is earned through customer sales and order fulfilment
- Gold is spent on: laboratory equipment, weapons, armour, raw materials (glassware if not blown, alcohol supply), and access to new map biomes
- Gold **never** functions as a time-skip mechanism — no premium speed-ups exist

---

## 11. The Ternary Alchemical Order

*SOLVE ET COAGULA*
*Mercurius · Sulphur · Sal | Operative Spagyrica*

The Ternary Alchemical Order is a hidden society of operative adepts whose identity and membership are never explicitly revealed within the game's main narrative. Their presence is felt only through correspondence — cryptic letters arriving sealed with their wax stamp.

The Order serves as the primary lore-delivery mechanism for players who do not already possess operative knowledge. Their letters are written in classical alchemical allegory, requiring the player to reason through the hint before applying it mechanically.

### 11.1 The Seal

- **Outer ring:** THE TERNARY ALCHEMICAL ORDER
- **Primary motto:** SOLVE ET COAGULA
- **Secondary descriptor:** MERCURIUS · SULPHUR · SAL | OPERATIVE SPAGYRICA
- **Central device:** Hexagram (Seal of Solomon) containing an upward triangle (Fire/Sulphur) overlaid with a downward triangle (Water/Mercury); a phoenix rising over flame at the centre
- **Crown:** Ouroboros serpent atop the inner ring
- **Planetary row:** ☉ ☽ ☿ ♂ ♃ ♀ ♄
- **In-game presentation:** Dark red wax stamp on letters, rendered at small scale — close-up inspection animation triggers on first receipt

### 11.2 Letter Progression

| Stage | Hint | Mechanic Targeted |
|---|---|---|
| Early | *"The spent plant is not waste, marry it with fire and water until it is pure."* | Directs away from compost bin toward calcination |
| Intermediate | *"That which rises in vapour carries the soul; that which remains is the body. Neither is complete without the other."* | Hints at recombination of tincture and salt |
| Advanced | *"The pelican feeds its young from its own breast; so too must your work return to its source seven times before it is worthy."* | Directs toward cohobation and the Pelican flask |

> *Note: The Ternary Alchemical Order is a real institution. Its seal and motto must be treated as IP belonging to the Order's founders. Final licensing terms to be confirmed prior to public release.*

---

## 12. Equipment & Progression Tiers

| Tier | Lab Equipment | Combat Gear | Milestones |
|---|---|---|---|
| **Tier 1 — The Forager** | Mortar & pestle, mason jars | Wooden stick | Local weeds; first slime encounters |
| **Tier 2 — The Herbalist** | Copper alembic, basic glass flasks | Dagger, leather armour | Minor elementals; first Spagyric tinctures |
| **Tier 3 — The Paracelsian** | Distillation train, Soxhlet extractor | Enchanted weapons | Wild beasts, mid-tier monsters; Magisteries |
| **Tier 4 — The Adept** | Pelican flask, full retort train, terrariums | Rare-drop enchanted armour | Plant Stones; Seven Oblations complete |

---

## 13. Technical Architecture

### 13.1 Language & Philosophy

Flora Philosophia is written in **C++**. This choice serves two purposes: performance for a real-time game with physics-based laboratory simulation, and as a deliberate learning vehicle — the project is developed in parallel with formal C++ study (*C++ All-In-One for Dummies*) so that theory and practice reinforce each other directly.

All source files must follow these conventions:

- **Clear section headers** in every file explaining what the module does and why
- **Inline comments** on any non-obvious logic, especially alchemical calculations, planetary hour math, and physics simulation
- **Descriptive variable and function names** — no single-letter variables outside of loop counters
- Code should be readable by someone learning C++ alongside the project, not just experienced engineers

### 13.2 Platform Targets

| Platform | Priority | Notes |
|---|---|---|
| **Web (browser)** | Primary | First build target; enables rapid iteration without device deployment |
| **Android** | Primary | Main mobile target; no Apple Developer ID or device available |
| **iOS** | Deferred | Added once an Apple Developer ID and test device are acquired |
| **Desktop (Windows/Linux)** | Secondary | Falls out naturally from the web/C++ architecture; useful for development |

### 13.3 Technology Stack

**Core engine:** C++ with a rendering and platform abstraction layer that compiles to both native (Android NDK) and web (WebAssembly via Emscripten).

| Layer | Technology | Purpose |
|---|---|---|
| **Language** | C++17 or C++20 | Core game logic, simulation, astrological engine |
| **Web compilation** | Emscripten | Compiles C++ to WebAssembly for browser deployment |
| **Rendering** | SDL2 + OpenGL ES 2.0 (or Raylib as a simpler alternative) | 2D rendering, input handling, audio; both compile to web and Android |
| **Android packaging** | Android NDK + Gradle | Wraps the C++ core into an Android APK |
| **Build system** | CMake | Cross-platform build configuration |
| **Save / state persistence** | JSON (nlohmann/json) or SQLite | Player save data, inventory, terrarium timers, lab queue |
| **GPS / clock** | Platform API calls (wrapped) | Planetary hour calculation; abstracted so the same logic works on web and Android |

> *Raylib is recommended as the starting rendering library. It is designed for learners, has excellent documentation, compiles cleanly to WebAssembly via Emscripten, and supports Android. It can be replaced later if the project outgrows it.*

### 13.4 Project Structure

A suggested top-level directory layout, designed for clarity during learning:

```
flora_philosophia/
├── src/
│   ├── main.cpp                  // Entry point
│   ├── core/
│   │   ├── game.cpp/h            // Main game loop and state manager
│   │   ├── clock.cpp/h           // Real-world clock, sunrise/sunset, planetary hours
│   │   └── save.cpp/h            // Save/load game state
│   ├── world/
│   │   ├── map.cpp/h             // Tile map, biomes, pathfinding
│   │   ├── player.cpp/h          // Player character, movement, stats
│   │   ├── enemy.cpp/h           // Enemy types, AI, drops
│   │   └── plant_node.cpp/h      // Harvestable plant nodes, quality calculation
│   ├── lab/
│   │   ├── laboratory.cpp/h      // Station management, sandbox interactions
│   │   ├── apparatus.cpp/h       // Individual apparatus logic (alembic, retort, etc.)
│   │   ├── residue.cpp/h         // Residue pipeline state machine
│   │   ├── glassblowing.cpp/h    // Glassblowing mini-game and skill tracking
│   │   └── timer.cpp/h           // Real-time processing timers
│   ├── alchemy/
│   │   ├── plant_db.cpp/h        // Plant data: name, ruler, Culpeper properties
│   │   ├── product.cpp/h         // Product types and quality modifiers
│   │   └── oblations.cpp/h       // Seven Oblations progression system
│   ├── combat/
│   │   ├── combat.cpp/h          // On-map combat loop
│   │   └── sigil.cpp/h           // Sigil drawing recognition and spell resolution
│   ├── economy/
│   │   ├── customer.cpp/h        // Customer types, request generation, dialogue
│   │   ├── vendor.cpp/h          // Town vendors, menstruum purchasing
│   │   └── mailbox.cpp/h         // Mail order system, Ternary Order letters
│   ├── idle/
│   │   └── terrarium.cpp/h       // Terrarium growth, hopper, planetary hour bonus
│   └── ui/
│       ├── hud.cpp/h             // In-world HUD, planetary hour display
│       ├── compendium.cpp/h      // Plant Compendium, grimoire illustrations
│       └── inventory.cpp/h       // Inventory management
├── assets/
│   ├── sprites/
│   ├── backgrounds/
│   ├── audio/
│   └── data/                     // JSON plant database, customer dialogue, lore letters
├── CMakeLists.txt
└── README.md
```

### 13.5 The Planetary Hour Engine (Core Module)

The `clock` module is the most critical and most educational component to build first. It does not depend on any rendering or game state and can be written, tested, and understood in isolation.

It must:
1. Accept latitude, longitude, and a UTC timestamp as inputs
2. Calculate local solar noon, sunrise, and sunset using the NOAA solar algorithm (well-documented, public domain)
3. Divide the day and night periods into 12 unequal segments each
4. Determine the current Planetary Hour index using the Chaldean order from local sunrise
5. Return the ruling planet, the hour's start and end time, and minutes remaining

This module is a pure C++ calculation — no graphics, no platform dependencies — making it the ideal first module to write while learning the language.

### 13.6 Development Sequence

Build in this order to maximise learning value and maintain a playable prototype at every stage:

1. **Planetary Hour Engine** — pure C++ logic; test in the terminal. No graphics required.
2. **Raylib window + player movement** — get a character moving on a tile map with tap/joystick input
3. **Plant nodes + harvest** — place nodes on the map, implement quality calculation using the clock module
4. **Laboratory basics** — maceration timer, residue pipeline state machine
5. **Glassblowing mini-game** — independent skill; implement as a self-contained module
6. **Menstruum system** — vendor purchase flow and basic fermentation timer
7. **Combat + sigil drawing** — on-map enemy encounters, sigil recognition
8. **Customer & economy system** — request generation, mailbox, Ternary Order letters
9. **Terrariums & idle layer** — passive generation, hopper collection
10. **Seven Oblations** — stat progression tied to Plant Stone completion
11. **Web build (Emscripten)** — compile existing codebase to WebAssembly; resolve platform differences
12. **Android packaging** — wrap the web/native build into an APK via Android NDK

---

## Open Design Questions

The following decisions remain open for future design sessions.

- **Party system depth:** Recruitable NPCs and/or open-world multiplayer are both under consideration. The core solo experience is confirmed; companion and multiplayer systems are stretch goals to be designed once the single-player loop is stable.
- **iOS support:** The primary targets are web and Android. iOS support is deferred until an Apple Developer ID and test device are available.
- **Multiplayer architecture:** If an open-world multiplayer mode is added, the networking model (peer-to-peer vs. dedicated server) and scope of shared world state will need a dedicated design session.

---

## Appendix A — Source Fidelity Note

The alchemical processes described in this document are drawn from the operative spagyric tradition as documented by Frater Albertus (*The Alchemist's Handbook*), Robert Allen Bartlett (*Real Alchemy*), and Nicholas Culpeper (*Culpeper's Complete Herbal*). Where the game makes a mechanical choice that diverges from historical practice for reasons of playability, this is noted explicitly in the relevant section.

The planetary attributions of herbs follow Culpeper's documented system throughout. This system is operatively recognised within the spagyric tradition and is not a game invention.

The Ternary system — Mercurius, Sulphur, Sal — is the foundational three-principle model of Paracelsian alchemy and forms the educational backbone of the entire product progression pipeline.

---

*☉ ☽ ♂ ☿ ♃ ♀ ♄*

*Solve Et Coagula*
