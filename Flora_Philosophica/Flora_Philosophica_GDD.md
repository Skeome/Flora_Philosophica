# Flora Philosophica
## Game Design Document — Version 0.3 — June 2026

*The Ternary Alchemical Order · Solve Et Coagula · Mercurius · Sulphur · Sal · Operative Spagyrica*

☉ ☽ ♂ ☿ ♃ ♀ ♄

---

## Table of Contents

1. [Core Concept](#1-core-concept)
2. [Visual Identity & Atmosphere](#2-visual-identity--atmosphere)
3. [The Real-Time Astrological Engine](#3-the-real-time-astrological-engine)
4. [The Humoral Temperament Engine](#4-the-humoral-temperament-engine)
5. [Natal Chart & Astrological Aspects](#5-natal-chart--astrological-aspects)
6. [The Laboratory](#6-the-laboratory)
7. [The Residue Purification Pipeline](#7-the-residue-purification-pipeline)
8. [Product Progression Ladder](#8-product-progression-ladder)
9. [Character Progression: The Seven Oblations](#9-character-progression-the-seven-oblations)
10. [Exploration & Combat](#10-exploration--combat)
11. [Idle Mechanics & Passive Generation](#11-idle-mechanics--passive-generation)
12. [Economy & Customer System](#12-economy--customer-system)
13. [The Ternary Alchemical Order](#13-the-ternary-alchemical-order)
14. [Equipment & Progression Tiers](#14-equipment--progression-tiers)
15. [Technical Architecture](#15-technical-architecture)
- [Appendix A — Source Fidelity Note](#appendix-a--source-fidelity-note)
- [Appendix B — Implementation Status](#appendix-b--implementation-status)
- [Open Design Questions](#open-design-questions)

---

## 1. Core Concept

Flora Philosophica is a cozy alchemical apothecary sanctuary game that functions as a stealth-educational guide to real-world plant alchemy. What presents itself as a fantasy apothecary simulator is, in practice, a technically accurate guide to operative spagyrics. A player who follows the in-game processes — maceration, calcination of the residue, leaching and precipitation of salts, and recombination — will successfully produce real herbal spagyric preparations in their own kitchen.

The game does not condescend to the player. There are no recipe locks, no tutorial pop-ups demanding completion before progression, and no premium speed-ups. Real-world knowledge is rewarded with immediate mechanical advantage.

### 1.1 Title

- Title: **Flora Philosophica**
- Considered alternatives: *Alchemilla's Hearth*, *Solve & Sprout*, *The Hermetic Herbalist*
- **Spagyricus** and **Spagyria** are reserved — both are chosen *nomina* of practising spagyrists (Robert Allen Bartlett and John H. Reid III respectively) and must not be used

### 1.2 Setting

- A secluded mountain cabin that evolves from a humble forager's hut into a master Alchemist's sanctuary
- The cabin is an explorable space — the player physically walks their character between laboratory stations, not through static menus
- As equipment is purchased and placed, the cabin visually fills and transforms, providing a tangible sense of progression
- Four explorable rooms: **Exterior**, **Cabin Main**, **Cabin Loft**, and **Garden Terrace**

### 1.3 The Hook

> *"What appears to be a casual fantasy apothecary simulator is actually a stealth-educational tool. If a player follows the game's recipes in real life, they will successfully create real herbal spagyrics."*

The game achieves this not through text explanations or tutorials but through the structure of its mechanics. The compost bin, the calcination failure states, the naming conventions of the residue pipeline, and the astrological timing system all teach operative spagyrics through play.

### 1.4 Player & Party

The player chooses from three characters at creation, each representing a gender option. The unchosen characters automatically populate the world as NPCs:

| Character | Gender | Notes |
|---|---|---|
| **Basil** | Masculine | Default protagonist; directional idle and walk animations implemented |
| **Lavender** | Feminine | Becomes NPC if not selected |
| **Sage** | Non-Binary | Becomes NPC if not selected |

- Recruitable NPCs are planned as a future addition — companions met through questlines who can assist in exploration, combat, or laboratory work
- Open-world multiplayer is under long-term consideration; the single-player loop will be fully designed and stable before any multiplayer scope is added

### 1.5 Character Creation

At character creation, the player provides:

1. **Character Selection** — Basil, Lavender, or Sage
2. **Geographic Location** — Latitude and longitude for the astrological engine (strictly local, never sent externally; defaults to Medford, Oregon 42.3265°N, 122.8756°W)
3. **Birth Data** — Date and time of birth (format: `YYYY-MM-DD HH:MM`) used to compute the player's Natal Chart via the VSOP87 engine, producing zodiac placements, planetary stat bonuses, and aspect-derived passive traits (see §5)

---

## 2. Visual Identity & Atmosphere

### 2.1 Perspective & Controls

- **3/4 top-down perspective** — pixel-art tile-based overworld (1920×1080 world canvas at 16px tiles, rendered at 640×360 native resolution) with y-sorted depth rendering
- **WASD Movement:** Direct 4-directional control at 80 px/s
- **Click-to-Move / Tap-to-Move:** `NavigationAgent2D` pathfinding around obstacles
- Scene transitions via door zones connect the Exterior, Cabin Main, Cabin Loft, and Garden
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

> *Status: Atmospheric overlays are designed but not yet implemented. The planetary hour data that drives them is fully operational.*

### 2.3 Botanical Illustrations

- Harvesting a plant triggers a full-screen pop-up styled as a historical grimoire page, influenced by Culpeper's *Complete Herbal*
- Each illustration details the plant's planetary ruler, elemental associations, humoral temperament (Choler, Sanguine, Phlegm, Melancholy values), and Culpeper's documented medicinal properties
- These illustrations populate the in-game **Plant Compendium** as discoveries are made

> *Status: Planned. The Plant Database backend is implemented with all required data fields.*

### 2.4 UI Design Direction

The HUD and inventory UI follow a *Stardew Valley*-inspired layout:

- **Clock HUD** (persistent, top-left) — Displays the current planetary hour with planet glyph (♄♃♂☉♀☿☽), color-coded by planet (Gold=Sun, Silver=Moon, Iron Red=Mars, Copper Green=Venus, Quicksilver=Mercury, Tin=Jupiter, Lead=Saturn), time remaining, hour/day ruler labels
- **Hotbar** along the bottom edge — quick access to harvested items, tools, and active preparations (10 slots)
- **Item slots** use simple icon + quantity format; quality tier indicated by a colored border
- **Station interaction** opens a contextual panel near the station, showing processing state, contents, and timer progress bar
- **Pause Menu** — Two-tab character sheet (Natal Chart + Analysis by Fire) alongside standard save/load/settings buttons

**Quality Tier Display:**

| Quality | Border Color | Condition |
|---|---|---|
| **Celestial** | Gold | Day ruler AND hour ruler match the plant's planet |
| **Pristine** | White | Hour ruler matches |
| **Standard** | Grey | Neutral alignment |
| **Stressed** | Orange | Hour ruler is the opposing planet |
| **Debased** | Dark | Both day AND hour are opposing planets |

### 2.5 Audio

- Tracker-styled compositions or classic hardware synthesis — a blend of nostalgic sample-based tracker music with modern atmospheric sound design
- Ambient audio layers shift with Planetary Hours, reinforcing the real-world timing system through sound as well as visuals
- Laboratory sounds are grounded: glass clinks, furnace roar, the hiss of a distillation train, the crystalline sound of salt precipitating

> *Status: Planned. No audio assets are currently integrated.*

---

## 3. The Real-Time Astrological Engine

Flora Philosophica synchronizes its internal clock entirely to the player's real world. There is no accelerated game calendar. This is the game's most significant design decision — it transforms the app from an entertainment product into a functional lifestyle companion for the operative spagyrist.

### 3.1 Planetary Hour Calculation

The C++ `PlanetaryHourCalculator` class is the core engine module. It:

1. Accepts **latitude, longitude, and a UTC timestamp** as inputs
2. Calculates local **solar noon, sunrise, and sunset** using a full solar position algorithm (Julian date, equation of time, solar declination, hour angle)
3. Divides the day and night periods into **12 unequal segments** each (Unequal Hours)
4. Determines the current **Planetary Hour index** using the **Chaldean order** from local sunrise: Saturn → Jupiter → Mars → Sun → Venus → Mercury → Moon
5. Returns a Dictionary containing: `ruling_planet`, `day_ruler`, `hour_index`, `minutes_remaining`, `planet_name`, `hour_start_utc`, `hour_end_utc`, `is_daytime`

Because of Unequal Hours, a planetary hour might last 75 real minutes in midsummer and shrink to 45 minutes in midwinter — teaching the player how seasons shift celestial timing as a natural consequence of engagement.

Default observer location: **Medford, Oregon** (42.3265°N, 122.8756°W) — overridden by the player's input at Character Creation, or GPS on mobile.

### 3.2 VSOP87 Celestial Mechanics

The C++ `PlanetaryOrbitCalculator` class provides a clean-room implementation of the **VSOP87 planetary theory**, calculating true geocentric ecliptic coordinates for all 7 classical planets. This powers:

- The main menu's **"dance of the spheres"** — an animated orrery with real retrograde loops, where Venus traces her 8-year pentagram and Saturn completes his 29.45-year circuit
- **Natal chart computation** — determining zodiac placements and inter-planetary aspects from the player's birth data
- Planet colors on the orrery: Saturn=Lead, Jupiter=Tin, Mars=Iron Red, Sun=Gold, Venus=Copper Green, Mercury=Quicksilver, Moon=Silver

### 3.3 Day and Hour Correspondence

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

### 3.4 Harvest Quality

Plant quality is determined by planetary alignment at the moment of harvest. Five quality tiers are defined in C++ (`HarvestItem::Quality`):

| Quality | Condition | Gameplay Effect |
|---|---|---|
| **Celestial** | Day ruler AND hour ruler match the plant's planet | Maximum product potency and sale price |
| **Pristine** | Hour ruler matches | High potency |
| **Standard** | Neutral alignment | Normal potency |
| **Stressed** | Hour ruler is the opposing planet | Reduced potency, increased failure risk |
| **Debased** | Both day AND hour are opposing planets | Lowest potency, highest failure probability |

No mechanic blocks access to any herb at any time — quality is the only modifier.

### 3.5 Consecration

- A preparation can be combined at any time after its processing window completes — but combining during the matching planetary day and hour stamps it as **Astro-Synchronised**
- Astro-Synchronised products command a significant price premium from elite customers and provide enhanced stat bonuses when consumed via the Seven Oblations
- Unsynchronised products are still valid and functional — this system rewards the disciplined player without punishing the casual one

---

## 4. The Humoral Temperament Engine

The humoral system is the mathematical backbone connecting historical pharmacopoeia to gameplay mechanics. It is sourced from Robert Allen Bartlett's *The Temper of Herbs* and implements the classical theories of Jabir ibn Hayyan, Al-Kindi, and Walter of Odington.

### 4.1 The Four Humors

Each plant in the database carries four **humoral values** stored as integers on the C++ `PlantData` class:

| Humor | Element | Dominant Quality | Secondary Quality |
|---|---|---|---|
| **Choler** | Fire 🜂 | Hot (4th degree) | Dry (3rd degree) |
| **Sanguine** | Air 🜁 | Wet (4th degree) | Hot (3rd degree) |
| **Phlegm** | Water 🜄 | Cold (4th degree) | Wet (3rd degree) |
| **Melancholy** | Earth 🜃 | Dry (4th degree) | Cold (3rd degree) |

Values follow **Jabir's subdivision scale** (0–28), where each of the 4 classical degrees is divided into 7 subdivisions (4 × 7 = 28), allowing fine-grained precision in temperament classification.

### 4.2 Al-Kindi's Geometric Temper Calculation

The GDScript function `GameManager.calculate_spagyric_temper()` implements Al-Kindi's geometric ratio mathematics using **Walter of Odington's intensity progressions**:

1. Each humor value is normalized: `val = humor / 7.0`
2. Geometric parts are computed using doubling progressions:
   - Hot parts = `2^(choler/7) + 2^(0.75 × sanguine/7)`
   - Cold parts = `2^(phlegm/7) + 2^(0.75 × melancholy/7)`
   - Wet parts = `2^(sanguine/7) + 2^(0.75 × phlegm/7)`
   - Dry parts = `2^(melancholy/7) + 2^(0.75 × choler/7)`
3. The ratio of opposing qualities is computed (Hot:Cold, Wet:Dry)
4. Logarithmic conversion yields the **classical degree** along each axis

The result is the plant's true **temperament** — e.g. "Hot in the 2nd degree, Dry in the 1st degree" — matching the historical pharmacopoeia. The degrees are based on a geometric progression of doublings, which is the pattern nature likes to do (as noted by Bartlett).

### 4.3 Compound Medicine Calculations

When multiple simples are combined into a compound medicine, the same calculation engine applies:

- Individual humoral parts from each ingredient are **summed**
- The combined parts are re-evaluated through Al-Kindi's ratio mathematics
- The compound's net temperament and degree are derived

This system is historically documented by **Ibn Biklarish** (who formulated equations to calculate qualities in compound medicines) and **Arnald de Villanova** (who perpetuated and expanded these methods).

> *Historical note: The degree ratios (1:1, 1:2, 1:4, 1:8, 1:16) correspond to the 4 degrees. A temperate medicine has equal parts of all 4 qualities. A medicine "Hot in the 1st degree" has a 2:1 ratio of hot parts to cold parts. The geometric progression continues through the 4th degree.*

### 4.4 Jabir's "Analysis by Fire" Data Sheet

Accessible from the Pause Menu's **Analysis by Fire** tab, this interactive data sheet implements the classical fire analysis protocol from Bartlett's *The Temper of Herbs*. The player selects any herb from the database dropdown to view:

| Display Section | Content |
|---|---|
| **Alchemical Separation of Elements** | 4-row grid: Phlegm/Water, White Spirit/Air, Red Spirit/Fire, Residue/Earth — each showing fraction name, alchemical element, weight percentage, and dominant + secondary classical qualities |
| **Derived Humoral Balance** | Calculated Hot/Cold and Wet/Dry degree (e.g. "Hot in the 2.31 degree, Dry in the 1.05 degree") |
| **Primary Quality Parts** | Raw geometric parts: Hot, Cold, Wet, Dry |
| **Jabir Subdivision Values** | Humoral values mapped back to Jabir's 28-point scale |

The fire analysis procedure (from page 123 of Bartlett): Place approximately 10 grams of powdered herb into a 25ml round-bottom distilling flask. Begin heating slowly to about 150°C — the clear "Phlegm" (water element) distills first, followed by the volatile spirits, leaving the fixed residue in the flask. Each fraction is weighed to determine the herb's elemental composition.

> *Status: The Analysis by Fire data sheet UI is implemented. A visual lab minigame for the fire analysis procedure (heating stages, fraction collection) is planned.*

---

## 5. Natal Chart & Astrological Aspects

### 5.1 Birth Data & Zodiac Placements

When the player enters their birth data at Character Creation, the system:

1. Parses the birth timestamp (`YYYY-MM-DD HH:MM`)
2. Uses the VSOP87 `PlanetaryOrbitCalculator` to compute true geocentric ecliptic longitudes for all 7 classical planets at the birth moment
3. Determines each planet's zodiac sign placement (ecliptic longitude ÷ 30° = sign index)
4. Grants a **flat +2 stat bonus** to the stat ruled by each sign's planetary ruler

The 12 zodiac signs and their rulers:

| Sign | Ruler | Sign | Ruler |
|---|---|---|---|
| Aries | Mars | Libra | Venus |
| Taurus | Venus | Scorpio | Mars |
| Gemini | Mercury | Sagittarius | Jupiter |
| Cancer | Moon | Capricorn | Saturn |
| Leo | Sun | Aquarius | Saturn |
| Virgo | Mercury | Pisces | Jupiter |

### 5.2 Astrological Aspects

Planetary aspects are computed from angular separations between planets with traditional orbs:

| Aspect | Angle | Orb | Type | Gameplay Effect |
|---|---|---|---|---|
| **Conjunction** | 0° | 8° | Synergy | +5% effectiveness to both planets' actions |
| **Sextile** | 60° | 6° | Harmony | +10% efficiency to both planets' actions |
| **Square** | 90° | 8° | Tension | +15% potency but +10% mental fatigue cost |
| **Trine** | 120° | 8° | Harmony | +10% efficiency to both planets' actions |
| **Opposition** | 180° | 8° | Tension | +15% potency but +10% mental fatigue cost |

Each detected aspect generates a **passive trait** (e.g. "Venus Trine Jupiter") that persists throughout gameplay. Traits are categorized as Synergy, Harmony, or Tension, influencing crafting, combat, and harvest outcomes.

### 5.3 Natal Chart Display

The Pause Menu's **Natal Chart** tab displays:

- **Planetary RPG Attributes** — A grid showing each planet's current stat value (base 10, modified by zodiac placements and Oblation consumption)
- **Natal Placements** — Each planet's zodiac sign and the resulting stat bonus
- **Astrological Passive Traits** — All active aspect traits with their names, types, and descriptions

If no birth data has been entered, the display prompts: *"No birth data loaded. Please define birth data during Character Selection."*

---

## 6. The Laboratory

The laboratory is not a menu. It is an explorable room within the mountain cabin. The player physically walks their character to each station to initiate actions, check progress, and collect results. As better equipment is purchased, it is placed in the lab space, which visually expands and fills across the four equipment tiers.

### 6.1 Real-Time Processing

- Processing uses **UTC timestamps** — laboratory timers continue whether the app is open or closed
- **There are no premium speed-ups.** This is non-negotiable. The pacing exists to respect the operative nature of the work.
- Players manage multiple preparations simultaneously — different projects at different stages — ensuring there is always something to act on during short play sessions

Processing times for the 9-step spagyric pipeline:

| Step | Process | Station | Time |
|---|---|---|---|
| 1 | Fresh → Dried | Drying Rack | 1 hour |
| 2 | Dried → Ground | Mortar & Pestle | 5 minutes |
| 3 | Ground → Tincture + Spent Residue | Maceration Jar | 1 day |
| 4 | Spent → Calx Black | Furnace | 30 minutes |
| 5 | Calx Black → Powdered Calx Black | Mortar & Pestle | 5 minutes |
| 6 | Powdered Calx Black → Calx Grey | Leaching Dish | 1 hour |
| 7 | Calx Grey → Calx White | Furnace | 30 minutes |
| 8 | Calx White → Salt Standard | Furnace | 30 minutes |
| 9 | Salt Standard → Purified Salt | Leaching Dish | 1 hour |

### 6.2 Station Interaction

Station interaction is handled via the `[E]` key:

1. The player approaches a station in the cabin
2. Pressing `[E]` with a valid item in the selected hotbar slot loads it into the station
3. The station automatically matches the item's current stage against its recipe pipeline (`RecipeDB.generate_pipeline_for_plant()`)
4. A contextual panel shows: current item, processing state (idle/processing/complete), and a timer progress bar
5. When complete, pressing `[E]` collects the result back into inventory

### 6.3 Sandbox Crafting

The laboratory operates as a true sandbox. There are no recipe locks graying out options. Any item can be placed into any apparatus. The game engine recognises the interaction and determines the result based on real chemical and alchemical logic.

- A player with prior knowledge of spagyrics can begin producing Spagyric Tinctures on Day 1 by ignoring the compost bin and calcining their first Residue immediately
- This sequence-breaking is rewarded with an immediate economic and progression advantage
- Invalid processing is still possible — burning something incorrectly yields **Generic Ash** rather than a named Salt — but the game does not prevent the attempt

### 6.4 The Compost Bin Trap

The compost bin is positioned outside the cabin and presented early as a natural destination for Spent Plant material. The game subtly encourages its use — it generates basic fertilizer for a small garden plot.

- Once any material touches the compost, it is **irretrievable** — effectively a thematic trash system
- There is no visible trash can, no explicit "delete item" button. The compost is the only disposal route for most players in the early game
- The revelation arrives via the Ternary Order letter system: *"The spent plant is not waste, marry it with fire and water until it is pure."*
- Players who have been faithfully composting experience the intended **"Aha!" moment** — recognizing that they have been discarding the Salt principle, the physical Body of the plant, for hours

### 6.5 Mini-Games

Two laboratory mini-games provide active engagement during processing:

**Furnace Mini-Game (Calcination):**
- The player controls temperature using a slider
- Three heat zones: **Safe** (slow processing), **Optimal** (fast processing), **Danger** (risk of failure)
- Visual cues: glass rattles audibly in the Danger zone
- **Minor heat excess:** Visual warning, player can reduce heat (recoverable)
- **Significant heat excess:** No warning — batch destroyed, yields Generic Ash
- Flask may crack if poorly managed

**Mortar & Pestle Mini-Game (Grinding):**
- The player taps/clicks in a circular motion to grind the material
- Accuracy and speed determine grinding quality
- Progress bar fills based on grinding precision
- Completion transitions the item to its ground stage

### 6.6 Failure States

The laboratory enforces physics-based consequences. There is no generic "Craft Failed" screen.

| Failure Type | Consequence |
|---|---|
| Minor heat excess | Visual warning: glass rattles audibly, liquid boils aggressively. Player has time to reduce heat. |
| Significant heat excess | No warning. Operation destroyed. Batch must be restarted. |
| Sealed flask with insufficient vapour room | Setup explodes, littering the lab floor with Broken Glass. |
| Invalid calcination | Still burns if flammable. Yields Generic Ash. The sandbox permits it; chemistry determines the result. |

### 6.7 The Glassblowing Station

- An optional purchasable station for the sanctuary, available from Tier 2 onward
- Glassblowing is an **independent skill** with its own experience track, separate from laboratory tier and character stats
- The player levels Glassblowing exclusively by practising it — blowing vessels, repairing broken glass, and attempting progressively complex shapes
- Glassblowing is a **mini-game**: the player controls breath pressure and rotation using on-screen input, shaping molten glass into the target vessel form. Accuracy determines the quality and tolerance rating of the finished piece.
- Allows the player to collect Broken Glass from explosions (or gather Sand from map biomes) and melt it down to hand-blow new vessels
- Higher Glassblowing skill unlocks more complex vessel types (retorts, pelicans, condensers) and produces pieces with higher heat and pressure tolerances than purchased glassware
- A skilled glassblower can produce equipment that is meaningfully superior to anything available from vendors — rewarding investment in the skill over time

> *Status: Planned. The `STAGE_BROKEN_GLASS` item stage exists in the C++ HarvestItem enum.*

---

## 7. The Residue Purification Pipeline

The Tria Prima — Mercurius (Spirit), Sulphur (Soul), and Sal (Body) — are the educational core of Flora Philosophica. The residue pipeline teaches the player to process the physical Body of the plant rather than discard it, completing the spagyric triad.

Item names update dynamically as the material changes state. The player's inventory is itself a progress tracker. All 22 stages are defined in the C++ `HarvestItem::PlantStage` enum:

### 7.1 Primary Pipeline (Stages 0–10)

| Stage | Enum | Item Name | Process | Notes |
|---|---|---|---|---|
| 0 | `STAGE_FRESH` | Fresh [Plant] | Harvested from the world | Quality determined by planetary alignment |
| 1 | `STAGE_DRIED` | Dried [Plant] | Drying Rack, 1 hr | Preserves the material for grinding |
| 2 | `STAGE_GROUND` | Ground [Plant] | Mortar & Pestle, 5 min | Prepared for maceration |
| 3 | `STAGE_TINCTURE` | [Plant] Tincture | Maceration Jar, 1 day | Mercurius + Sulphur extracted; Residue produced simultaneously |
| 4 | `STAGE_SPENT` | Spent [Plant] / Residue | Byproduct of maceration | Many players discard to the compost bin here — this is the "trap" |
| 5 | `STAGE_CALX_BLACK` | Caput Mortuum | Furnace, 30 min | First burn stage; vessel may crack if heat mismanaged |
| 6 | `STAGE_CALX_BLACK_GROUND` | Powdered Calx Black | Mortar & Pestle, 5 min | Ground for leaching |
| 7 | `STAGE_CALX_GREY` | [Plant] Calx (Grey) | Leaching Dish, 1 hr | First leaching with distilled water |
| 8 | `STAGE_CALX_WHITE` | [Plant] Calx (White) | Furnace, 30 min | Further calcination to clarify |
| 9 | `STAGE_SALT` | Salt of [Plant] | Furnace, 30 min | Crystalline salt formed |
| 10 | `STAGE_SALT_PURIFIED` | Purified Salt of [Plant] | Leaching Dish, 1 hr | Ready for recombination |

### 7.2 Advanced Products (Stages 11–16)

| Stage | Enum | Product | Method |
|---|---|---|---|
| 11 | `STAGE_SPAGYRIC` | Spagyric Tincture | Basic tincture + purified Salt recombined |
| 12 | `STAGE_ELIXIR` | Elixir | Advanced distillation via retort apparatus |
| 13 | `STAGE_MAGISTERY` | Magistery | Extended distillation process |
| 14 | `STAGE_ENS` | Ens Tincture | Extracted using deliquesced salts (Sal Tartar) |
| 15 | `STAGE_PRIMUM_ENS` | Primum Ens | Further refinement of Ens |
| 16 | `STAGE_PLANT_STONE` | Plant Stone | Cohobation in Pelican flask |

### 7.3 Auxiliary Items (Stages 17–21)

| Stage | Enum | Item | Source |
|---|---|---|---|
| 17 | `STAGE_GENERIC_ASH` | Generic Ash | Failed calcination |
| 18 | `STAGE_BROKEN_GLASS` | Broken Glass | Flask explosions; raw material for glassblowing |
| 19 | `STAGE_MENSTRUUM` | Menstruum | Purchased or home-distilled alcohol |
| 20 | `STAGE_FERTILIZER` | Fertilizer | Compost bin output |
| 21 | `STAGE_SEED` | Seed | For planting |

---

## 8. Product Progression Ladder

Players with no prior knowledge begin at the bottom. Players with existing operative knowledge may begin anywhere — the sandbox system does not gate access. Customer orders scale to match the player's demonstrated product level.

| Product | Method | Notes |
|---|---|---|
| **Basic Tincture** | Maceration of herb in alcohol. 1 real day. | Entry level. Most early customers request these. |
| **Spagyric Tincture** | Basic tincture + purified Salt of [Plant] recombined. | Requires completing the full residue purification pipeline. |
| **Elixirs & Magisteries** | Advanced distillation using retort apparatus. | Unlocks Alchemist and Noble customers. |
| **Ens Tincture & Primum Ens** | Essence extracted using deliquesced salts (Sal Tartar). | High-value mail orders; rare and time-intensive. |
| **Plant Stone** | Ultimate masterwork via cohobation in a Pelican flask. | One per plant; permanent. Required for Seven Oblations completion. |

### 8.1 The Plant Stone

The Plant Stone is the endgame masterwork for each individual plant. It is achieved through **cohobation** — the cyclic process of continuous distillation and recombination using a Pelican flask, which feeds its own distillate back over the fixed base repeatedly.

- One Plant Stone per plant — each is a permanent, discrete item
- The Pelican flask becomes available at Tier 4 and is required for cohobation
- Completing a Plant Stone is the prerequisite for finalising a Seven Oblations entry for that plant's planetary ruler
- Plant Stones command the highest prices from elite customers and are requested by Wizards, Nobles, and Ternary Order-affiliated NPCs

---

## 9. Character Progression: The Seven Oblations

Flora Philosophica has no experience point bar. Character stats are increased exclusively through an internal alchemy system called the **Seven Oblations**. The player refines plants ruled by the Seven Planets and consumes the resulting spagyric products to level up the corresponding RPG stat.

### 9.1 Planetary Stat Mapping

Stats begin at a base of **10** and are modified by natal chart placements (+2 per zodiac sign bonus) and Oblation consumption:

| Planetary Ruler | RPG Stat | Harvest Notes |
|---|---|---|
| **☉ Sun** | Vitality / Max HP | Solar herbs; harvest Sunday Solar hour |
| **☽ Moon** | Mana / MP | Monday; lunar hour near midnight |
| **♂ Mars** | Attack Power / Phys DMG | Tuesday; harvesting in iron-red glow |
| **♀ Venus** | Charisma / Shop Prices | Friday; copper-green ambient light |
| **☿ Mercury** | Agility / Evasion | Wednesday; quicksilver shimmer |
| **♃ Jupiter** | Luck / Crit Chance | Thursday; indigo sky tint |
| **♄ Saturn** | Defense / Armor | Saturday; leaden shadows lengthen |

> *Culpeper's planetary attributions are used throughout. These are historically documented and operatively recognised within the spagyric tradition. The list given in each planetary category is not exhaustive — players may choose any Culpeper-attributed plant for that ruler.*

### 9.2 Product XP Scaling

There is no "pledge" or lock-in; players can craft and consume products from any plant at any time. Every completed spagyric product acts as an "EXP Book" for its ruling planet's stat, scaling by the complexity of the laboratory process:

- **Spagyric Tincture:** Extra Small XP
- **Elixir:** Small XP
- **Magistery:** Medium XP
- **Ens Tincture:** Large XP
- **Primum Ens:** Extra Large XP
- **Plant Stone:** Extra Extra Large XP

Different plants offer different base progression scaling. For example, a common Solar plant like Rosemary may be easy to cultivate but yields less XP than a rare, difficult-to-process Solar plant like Angelica.

### 9.3 Astrological Consumption & Plant Stones

The timing of consumption is just as critical as the harvest. The game relies on the real-time Planetary Engine to determine the effect of consuming an Oblation:

| Consumption Timing | Effect on Stat |
|---|---|
| **Matching Day & Hour** | Maximum Permanent Stat Boost |
| **Matching Hour Only** | Minor Permanent Stat Boost |
| **Neutral (No Match)** | Temporary Stat Boost (No permanent XP gained) |
| **Opposite Day/Hour** | Temporary Stat Debuff |

**The Plant Stone:**
Unlike lower-tier products which are consumed and destroyed, the **Plant Stone** is a discrete, permanent item. Only one Plant Stone can be crafted per specific plant species. When used for an Oblation, it does not disappear. However, to prevent stat-spamming, each Plant Stone has a significant real-world cooldown (e.g., 1 real week) between uses. A disciplined player must wait for the perfect celestial window to use their Stone for maximum permanent gain.

> *Status: Planned. The Plant Stone stage exists in the item enum; the Oblation consumption and cooldown system is not yet implemented.*

---

## 10. Exploration & Combat

### 10.1 The Map & Meditation

- The physical world is navigated via a 3/4 top-down perspective, focusing on resource gathering, exploration, and finding rare plant nodes.
- **There is no physical combat in the overworld.** The game maintains its grounded, peaceful atmosphere in the physical realm.
- Combat is initiated via **Meditation ("Guided Imagery")**. The player finds specific nodes or quiet spots in the world, sits, and enters the "Spirit World" within their mind.
- This grounds the magic system — the fantasy stays within the imagination, allowing for spectacular "real" magic without breaking the low-fantasy alchemical setting of the physical world.

### 10.2 The Spirit World Enemies

Within the meditation state, the player encounters entities that do not exist in the physical exploration:
- **Spiritual Essences:** Literal monsters (Slimes, Wolves, Goblins, Elementals) whose spirits reside in this mental realm.
- **Inner Demons:** Shadow clones (akin to "Dark Link") that possess the player's exact stats, skills, and sigil knowledge, serving as high-level boss encounters.

### 10.3 Sigil-Based Combat

Combat inside the meditation state relies on drawing sigils on the screen using touch input.
- The player's magical arsenal grows via the Seven Oblations. They unlock complex planetary sigils (e.g., compounding Mars with a Fire triangle) and entirely non-planetary alchemical symbols (like the classic Philosopher's Stone grid or complex Castlevania-style point-based glyphs).
- **Planetary Hour Modifiers:** The current real-world planetary hour heavily influences spell efficacy, mimicking plant harvest quality:
  - **4.0x Damage:** Matching Day and Hour
  - **2.0x Damage:** Matching Hour only
  - **1.0x Damage:** Neutral (no match)
  - **0.5x Damage:** Opposite Hour only
  - **0.25x Damage:** Opposite Day and Hour
- **Natal Aspect Modifiers:** Active aspect traits from the player's natal chart also apply:
  - **Synergy traits** grant +5% effectiveness to associated planetary sigils
  - **Harmony traits** grant +10% efficiency (reduced mana cost)
  - **Tension traits** grant +15% potency but increase mental fatigue cost by +10%

### 10.4 Defeat Consequences

- If defeated in the Spirit World, the player does not die. They "wake up" immediately at their physical meditation spot.
- **Penalties:** A loss of in-game Time and Stamina, accompanied by a **"Mental Fatigue"** debuff.
- **Mental Fatigue:** Significantly lowers laboratory crafting success rates for a few real-time hours, reinforcing the connection between spiritual focus and operative spagyric success.

> *Status: The combat system is scaffolded (`src/combat/` directory exists) but not yet implemented.*

### 10.5 Multiplayer Integration

Multiplayer expands on the meditation state. It is a collaborative, trade, and battle-focused system.
- Players can engage in **co-op dungeons and boss-rushes** within the Spirit World.
- Because the planetary engine relies on the player's local device time, asynchronous advantages occur: It might be a Saturn hour for Player A (buffing defense) and a Mars hour for Player B (buffing attack) simultaneously, creating a strategic "barter system" of time zones and buffs during co-op play.

> *Status: Long-term goal. Not designed or implemented.*

---

## 11. Idle Mechanics & Passive Generation

Flora Philosophica does not use an away-grinding system for the player character. The idle layer is entirely contained within the Sanctuary (the mountain cabin and its grounds).

### 11.1 Enchanted Terrariums

- Passive resource generators constructed and placed within the sanctuary
- Each terrarium is configured for a specific plant and slowly grows and auto-harvests that plant over time, depositing cuttings into a collection hopper
- Growth rate is subtly influenced by real-world Planetary Hours — a Sun terrarium growing St. John's Wort grows faster during Solar hours
- Tending the terrarium during the correct planetary hour grants a quality bonus to the next harvest
- Terrariums unlock once the player has made at least one successful wild harvest of the target plant

> *Status: Planned. The `ITEM_TERRARIUM` apparatus type and `STAGE_FERTILIZER`/`STAGE_SEED` item stages exist in C++ but no GDScript logic is implemented.*

### 11.2 The Idle Loop

The laboratory continues processing any active preparations whether the app is open or closed (via UTC timestamp-based timers).

1. Player logs in, collects terrarium hoppers and any completed laboratory preparations
2. Starts new macerations, calcinations, or distillations — setting multiple parallel projects running
3. Spends active session time exploring the map, fighting enemies, fulfilling customer orders, or tending the lab
4. Logs off with several projects running simultaneously

> *Designed for short, satisfying daily sessions of 5–15 minutes of active engagement, with passive processing running continuously in the background.*

### 11.3 Notification System

- *"Your St. John's Wort tincture has finished macerating."*
- *"Your terrarium has produced 8 Mugwort cuttings — collect them before the hopper fills."*
- *"A Mars Hour begins in 10 minutes — your Nettle nodes are glowing."*
- *"A new letter has arrived at the cabin."*

> *Status: Planned.*

---

## 12. Economy & Customer System

### 12.1 Customer Types & Delivery Methods

Customer interaction evolves as the sanctuary's reputation grows:
1. **Mailbox Post (Early Game):** The primary interface. Villagers and distant contacts drop request letters here.
2. **Village Message Board (Fallback):** For villagers unaware of your direct services; the player must walk to town to read these open requests.
3. **The Storefront (Mid-to-Late Game):** A dedicated room in the cabin (or purchased outbuilding) where products are physically displayed. Customers visit directly.

| Customer | Request Complexity | Reward | Notes |
|---|---|---|---|
| **Villagers** | Simple remedies (Tinctures) | Modest gold | Usually via mailbox or town board. |
| **Adventurers' Guild** | Elemental materials, combat preps | Moderate gold | Steady source of radiant requests. |
| **Nobles** | Elixirs and Magisteries | High gold | Very high affluence multiplier. |
| **Visiting Alchemists** | Rare products, Spagyric work | High gold | Often trigger story progression. |
| **Wizards** | Ens Tinctures and Plant Stones | Massive payouts | Endgame handcrafted requests. |
| **Ternary Order Members** | Cryptic requests, lore hints | Lore & Secrets | "Disguised" customers; their dialogue uses the special UI font (*Archemy.otf*). |

### 12.2 Request Generation & Pricing

**Request Generation:** The game uses a hybrid approach. Procedurally generated "radiant" requests provide a steady stream of income based on the player's current equipment tier. These are interspersed with handcrafted story requests that advance the narrative, introduce new mechanics, or unlock new customer tiers.

**Pricing Model:** Products have fixed base values, but the final sale price is determined automatically by multipliers:
`Final Price = Base Value × Harvest Quality Multiplier × Customer Affluence Multiplier`
(e.g., A Celestial-quality tincture sold to a Noble pays out significantly more than a Standard tincture sold to a Villager.)

### 12.3 Menstruum (Alcohol) Sourcing

The menstruum — the alcoholic solvent used for maceration and tincture preparation — can be obtained in two ways, reflecting short-term and long-term play strategies.

- **Purchase from vendors (short-term):** Basic spirits are available for purchase from town vendors. This allows new players to begin laboratory work immediately without an agriculture prerequisite. Vendor spirits are of Standard quality and carry no special properties.
- **Home fermentation and distillation (long-term):** Players can invest in Grape Vine or Wheat seeds, cultivate a small plot on the cabin grounds, ferment the harvest, and distill their own menstruum using in-lab apparatus. Home-distilled spirits are of higher quality, carry a celestial stamp if produced under the correct Planetary Hours, and cost only time and labour after the initial seed investment. This is the authentic operative path and is rewarded with better final product quality and customer pricing.

> *Status: Planned. The `ITEM_MAILBOX_POST` and `ITEM_STORAGE_CHEST` apparatus types exist in C++; the `STAGE_MENSTRUUM` item stage is defined. No economy GDScript logic is implemented.*

---

## 13. The Ternary Alchemical Order

*SOLVE ET COAGULA*
*Mercurius · Sulphur · Sal | Operative Spagyrica*

The Ternary Alchemical Order is a hidden society of operative adepts whose identity and membership are never explicitly revealed within the game's main narrative. Their presence is felt only through correspondence — cryptic letters arriving sealed with their wax stamp.

The Order serves as the primary lore-delivery mechanism for players who do not already possess operative knowledge. Their letters are written in classical alchemical allegory, requiring the player to reason through the hint before applying it mechanically.

### 13.1 The Seal

- **Outer ring:** THE TERNARY ALCHEMICAL ORDER
- **Primary motto:** SOLVE ET COAGULA
- **Secondary descriptor:** MERCURIUS · SULPHUR · SAL | OPERATIVE SPAGYRICA
- **Central device:** Hexagram (Seal of Solomon) containing an upward triangle (Fire/Sulphur) overlaid with a downward triangle (Water/Mercury); a phoenix rising over flame at the centre
- **Crown:** Ouroboros serpent atop the inner ring
- **Planetary row:** ☉ ☽ ☿ ♂ ♃ ♀ ♄
- **In-game presentation:** Dark red wax stamp on letters, rendered at small scale — close-up inspection animation triggers on first receipt
- The TTAO info panel is accessible from the Main Menu using the *Archemy.otf* alchemical Unicode font

### 13.2 Letter Progression

| Stage | Hint | Mechanic Targeted |
|---|---|---|
| Early | *"The spent plant is not waste, marry it with fire and water until it is pure."* | Directs away from compost bin toward calcination |
| Intermediate | *"That which rises in vapour carries the soul; that which remains is the body. Neither is complete without the other."* | Hints at recombination of tincture and salt |
| Advanced | *"The pelican feeds its young from its own breast; so too must your work return to its source seven times before it is worthy."* | Directs toward cohobation and the Pelican flask |

> *Note: The Ternary Alchemical Order is a real institution. Its seal and motto must be treated as IP belonging to the Order's founders. Final licensing terms to be confirmed prior to public release.*

---

## 14. Equipment & Progression Tiers

Equipment placement is managed by the C++ `RoomManager` class across 4 rooms. Each apparatus has tile dimensions, a tier requirement, and indoor/outdoor placement flags.

### 14.1 Tier Progression

| Tier | Lab Equipment | Combat Gear | Milestones |
|---|---|---|---|
| **Tier 1 — The Forager** | Mortar & pestle, mason jars, drying rack, fireplace | Wooden stick | Local weeds; first slime encounters |
| **Tier 2 — The Herbalist** | Copper alembic, basic glass flasks, glassblowing station | Dagger, leather armour | Minor elementals; first Spagyric tinctures |
| **Tier 3 — The Paracelsian** | Distillation train, Soxhlet extractor | Enchanted weapons | Wild beasts, mid-tier monsters; Magisteries |
| **Tier 4 — The Adept** | Pelican flask, full retort train, terrariums | Rare-drop enchanted armour | Plant Stones; Seven Oblations complete |

### 14.2 Apparatus Catalogue

19 apparatus types are defined in the C++ `ItemDB` class:

| # | Apparatus | Tier | Indoor | Notes |
|---|---|---|---|---|
| 0 | Fireplace | 1 | Yes | Heat source for basic operations |
| 1 | Drying Rack | 1 | Both | Fresh → Dried |
| 2 | Mortar & Pestle | 1 | Yes | Grinding mini-game; multiple pipeline steps |
| 3 | Maceration Jar | 1 | Yes | Ground → Tincture + Residue |
| 4 | Compost Bin | 1 | No | The "trap" — irretrievable disposal |
| 5 | Work Bench | 1 | Yes | Crafting surface |
| 6 | Copper Alembic | 2 | Yes | Basic distillation |
| 7 | Glass Flask | 2 | Yes | Laboratory vessel |
| 8 | Glassblowing Station | 2 | Yes | Independent skill mini-game |
| 9 | Distillation Train | 3 | Yes | Advanced distillation |
| 10 | Soxhlet Extractor | 3 | Yes | Continuous extraction |
| 11 | Pelican Flask | 4 | Yes | Cohobation for Plant Stones |
| 12 | Retort Train | 4 | Yes | Advanced processing |
| 13 | Terrarium | 2 | Both | Passive plant generation |
| 14 | Bookshelf | 1 | Yes | Lore and reference storage |
| 15 | Storage Chest | 1 | Yes | Extended item storage |
| 16 | Mailbox Post | 1 | No | Customer order interface |
| 17 | Furnace | 1 | Yes | Calcination mini-game; multiple pipeline steps |
| 18 | Leaching Dish | 1 | Yes | Salt purification |

---

## 15. Technical Architecture

### 15.1 Engine & Language

Flora Philosophica is built with **Godot 4.7** and a **C++17 GDExtension** backend. Core data models, calculations, and persistence are written in C++; game logic, UI, natal chart computation, Al-Kindi temper calculations, and scene management are in GDScript.

The C++ layer registers **13 classes** into Godot's ClassDB:

| Category | Classes |
|---|---|
| **Core** | `PlanetaryHourCalculator`, `PlanetaryOrbitCalculator` |
| **World** | `HarvestItem`, `ItemDefinition`, `ItemDB`, `InventorySlot`, `Inventory`, `PlacedItem`, `RoomManager` |
| **Alchemy** | `PlantData`, `PlantDatabase`, `Recipe`, `Station` |

### 15.2 Platform Targets

| Platform | Priority | Notes |
|---|---|---|
| **Linux** | Primary | Developed on Arch Linux w/ Hyprland/Wayland |
| **Windows** | Supported | CMake produces `.dll` |
| **macOS** | Supported | CMake produces `.dylib` |
| **Android** | Build script ready | `scripts/build_android.sh` |
| **WebAssembly** | Build script ready | `scripts/build_web.sh` |
| **iOS** | Deferred | Added once an Apple Developer ID and test device are acquired |

### 15.3 Build System

CMake handles dependencies automatically via [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake):
- `godot-cpp` 10.0.0-rc1
- `nlohmann_json` 3.11.3

```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

The compiled library is placed at `godot/bin/libflora_philosophica.so`.

### 15.4 Project Structure

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
│   └── combat/  economy/  idle/  lab/  # Scaffolded — reserved for future systems
│
├── godot/                          # Godot 4.7 project (GL Compatibility)
│   ├── scenes/
│   │   ├── main_menu.tscn              # Title screen w/ VSOP87 planet orbits
│   │   ├── character_selection.tscn    # Character, location, birth data input
│   │   ├── main.tscn                   # Overworld (Ground, Walls, Rooftops,
│   │   │                               #   Counter-tops, Player, HUD)
│   │   ├── cabin_main.tscn            # Interior cabin with station nodes
│   │   ├── player.tscn                 # "Basil" — CharacterBody2D, 4-dir anims
│   │   ├── plant_node.tscn             # Harvestable plant (quality from alignment)
│   │   ├── station_node.tscn           # Interactive lab station wrapper
│   │   ├── clock_hud.tscn              # Planetary hour display (♄♃♂☉♀☿☽)
│   │   ├── pause_menu.tscn             # Non-pausing overlay: Natal Chart +
│   │   │                               #   Analysis by Fire tabs
│   │   ├── minigame_furnace.tscn       # Temperature-control calcination game
│   │   └── minigame_mortar.tscn        # Circular-grinding mortar game
│   ├── scripts/
│   │   ├── game_manager.gd             # Autoload: save/load, C++ object refs,
│   │   │                               #   natal chart, aspects, Al-Kindi temper calc
│   │   ├── character_selection.gd      # Character/location/birth data → GameManager
│   │   ├── player.gd                   # WASD + click-to-move, animations
│   │   ├── world.gd                    # Door transitions, auto-save
│   │   ├── cabin.gd                    # Station placement, [E] interaction
│   │   ├── station_node.gd             # C++ Station wrapper, recipe matching
│   │   ├── plant_node.gd               # Growth stages, astrological quality
│   │   ├── recipe_db.gd                # 9-step spagyric pipeline generator
│   │   ├── clock_hud.gd                # Planet glyphs, color coding, timer
│   │   ├── main_menu.gd                # Menu, settings, TTAO panel, credits
│   │   ├── pause_menu.gd              # Natal Chart tab + Analysis by Fire tab
│   │   ├── minigame_furnace.gd         # Temperature zones, failure states
│   │   ├── minigame_mortar.gd          # Grinding accuracy & progress
│   │   └── planet_orbit_animator.gd    # VSOP87 orrery visualization (377 lines)
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
│   ├── Flora_Philosophica_GDD.md       # This document (v0.3)
│   ├── Cpp_GD_Roadmap.md              # Technical architecture roadmap
│   ├── The Cabin/                      # Location design (Cabin, Lab, Storefront)
│   ├── The Ternary Alchemical Order/   # Faction lore
│   ├── Plants/                         # Per-plant documentation (planned)
│   ├── TAO_Seal.png                    # Faction seal artwork
│   └── Flora Philosophica.canvas       # Obsidian design board
│
├── BartlettTemperofHerbs.txt       # OCR text — R.A. Bartlett, The Temper of Herbs
├── BartlettTemperofHerbs.pdf       # Source PDF
├── TemperofHerbsImages/            # Scanned pages (142 images) — tables, diagrams
├── Culpepers_Complete_Herbal.epub  # Primary botanical reference source
├── tests/                          # Test scaffolding (mirrors src/ structure)
├── scripts/                        # Build & run helpers
│   ├── build_android.sh                # Android cross-compilation
│   ├── build_web.sh                    # WebAssembly compilation
│   └── run_game.sh                     # Wayland/Hyprland compatibility wrapper
├── CMakeLists.txt                  # CMake + CPM build system
└── save.json                       # Save data (JSON format)
```

### 15.5 The Planetary Hour Engine (Core Module)

The `PlanetaryHourCalculator` class is the most critical and most educational component. It is a pure C++ calculation with no graphics or platform dependencies.

It:
1. Accepts latitude, longitude, and a UTC timestamp as inputs
2. Calculates local solar noon, sunrise, and sunset using a full solar position algorithm (Julian date, equation of time, solar declination, hour angle)
3. Divides the day and night periods into 12 unequal segments each
4. Determines the current Planetary Hour index using the Chaldean order from local sunrise
5. Returns the ruling planet, the hour's start and end time, minutes remaining, day ruler, and daytime flag

### 15.6 Save System

Full save/load system using JSON (`user://save.json`) tracking:
- 46-slot inventory (10 hotbar + 36 grid) with full item serialization
- Placed apparatus across 4 rooms
- Player position (x, y)
- Observer latitude/longitude
- Player gender and birth data
- All station processing states with UTC timestamps (continues offline)
- Auto-saves on room transitions and exit

### 15.7 Key GDScript Systems

| System | File | Responsibility |
|---|---|---|
| **GameManager** | `game_manager.gd` | Autoload singleton; holds C++ object references, save/load, natal chart computation, aspect detection, Al-Kindi temper calculation |
| **RecipeDB** | `recipe_db.gd` | Generates the 9-step spagyric pipeline as an array of `Recipe` objects |
| **PauseMenu** | `pause_menu.gd` | Two-tab character sheet: Natal Chart (stats, placements, traits) and Analysis by Fire (plant selection, elemental separation, temper calculation) |
| **StationNode** | `station_node.gd` | Wraps C++ `Station` for in-scene use; handles recipe matching, visual state changes, and [E] interaction |
| **Cabin** | `cabin.gd` | Manages station placement and interaction in the cabin scene |
| **PlanetOrbitAnimator** | `planet_orbit_animator.gd` | 377-line VSOP87 orrery visualization on the main menu |

---

## Appendix A — Source Fidelity Note

The alchemical processes described in this document are drawn from the operative spagyric tradition as documented by Frater Albertus (*The Alchemist's Handbook*), Robert Allen Bartlett (*Real Alchemy* and *The Temper of Herbs*), and Nicholas Culpeper (*Culpeper's Complete Herbal*). Where the game makes a mechanical choice that diverges from historical practice for reasons of playability, this is noted explicitly in the relevant section.

The planetary attributions of herbs follow Culpeper's documented system throughout. This system is operatively recognised within the spagyric tradition and is not a game invention.

The humoral temperament calculations follow the historical chain of transmission: **Jabir ibn Hayyan** (elemental subdivisions into 28 parts), **Al-Kindi** (geometric ratio scale using doubling progressions), **Walter of Odington** (intensity progressions mapping qualities to elements), **Ibn Biklarish** (compound medicine calculations), and **Arnald de Villanova** (perpetuation and expansion of these methods). Bartlett's *The Temper of Herbs* is the primary modern reference synthesizing these traditions.

The Tria Prima — Mercurius, Sulphur, Sal — are the foundational three-principle model of Paracelsian alchemy and forms the educational backbone of the entire product progression pipeline.

---

## Appendix B — Implementation Status

A snapshot of what is implemented vs. planned as of June 2026:

### ✅ Fully Implemented
- Planetary Hour Engine (C++ `PlanetaryHourCalculator`)
- VSOP87 Celestial Mechanics (C++ `PlanetaryOrbitCalculator`) + main menu orrery
- Plant Database: 100+ herbs with planetary ruler, element, Culpeper properties, and 4-humor temper values
- 22-stage HarvestItem pipeline (Fresh → Plant Stone + failure states)
- 5-tier quality system (Debased → Celestial)
- 46-slot inventory with full serialization
- 4-room placement system (Exterior, Cabin, Loft, Garden)
- 9-step spagyric recipe pipeline with real-time UTC timers
- Station processing (loads item, matches recipe, ticks in real-time, collects result)
- Character creation (3 characters, location override, birth data)
- Natal chart computation (VSOP87 positions → zodiac placements → stat bonuses)
- Astrological aspect detection (Conjunction, Sextile, Square, Trine, Opposition)
- Passive trait generation from aspects (Synergy, Harmony, Tension effects)
- Al-Kindi geometric temper calculation (`calculate_spagyric_temper()`)
- Analysis by Fire data sheet (Pause Menu tab)
- Natal Chart display (Pause Menu tab)
- Main menu with VSOP87 orrery, settings panel, credits, TTAO panel
- Clock HUD with planet glyphs, color coding, and time remaining
- Player movement (WASD + click-to-move pathfinding + 4-dir animations)
- Door zone scene transitions with auto-save
- Furnace mini-game (temperature control, 3 heat zones, failure states)
- Mortar & pestle mini-game (circular grinding, accuracy tracking)
- Cabin station interaction system ([E] key loading/collecting)
- Full JSON save/load system (offline-capable timers)
- 19 apparatus type definitions with tile dimensions and tier requirements

### 🔲 Scaffolded / Partially Implemented
- `src/combat/` — Directory exists, no C++ code
- `src/economy/` — Directory exists, no C++ code
- `src/idle/` — Directory exists, no C++ code
- `src/lab/` — Directory exists (station logic lives in `src/alchemy/`)
- Graphical inventory UI — C++ backend complete, no visual UI
- Atmospheric overlays — Planetary hour data available, no visual effects

### ❌ Not Yet Implemented
- Sigil-based combat system
- Economy & customer system (request generation, pricing, storefronts)
- Seven Oblations progression system (consumption, cooldowns, stat gains)
- Terrarium idle mechanics
- Glassblowing mini-game
- Menstruum fermentation/distillation
- Plant Compendium UI
- Audio system (music, ambient, laboratory sounds)
- Notification system
- Compound medicine UI (multi-herb recipe creation)
- Visual lab minigame for fire analysis procedure
- NPC companions
- Multiplayer

---

## Open Design Questions

The following decisions remain open for future design sessions.

- **Compound medicine UI:** The Al-Kindi calculation engine supports compound medicines, but the UI for selecting multiple herbs and viewing the combined temperament is not yet designed. Should this be a dedicated crafting interface or an extension of the Analysis by Fire sheet?
- **Aspect trait application:** Aspect traits are computed and displayed but not yet mechanically applied to crafting, combat, or harvest. The exact modifier formulas need to be finalised.
- **Party system depth:** Recruitable NPCs and/or open-world multiplayer are both under consideration. The core solo experience is confirmed; companion and multiplayer systems are stretch goals to be designed once the single-player loop is stable.
- **iOS support:** The primary targets are Linux, Windows, macOS, Android, and WebAssembly. iOS support is deferred until an Apple Developer ID and test device are available.
- **Multiplayer architecture:** If an open-world multiplayer mode is added, the networking model (peer-to-peer vs. dedicated server) and scope of shared world state will need a dedicated design session.

---

*☉ ☽ ♂ ☿ ♃ ♀ ♄*

*Solve Et Coagula*
