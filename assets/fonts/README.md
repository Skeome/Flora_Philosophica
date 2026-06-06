# assets/fonts/

This directory contains proprietary font files that are **not committed to the repository**.

Font files are listed in `.gitignore`. After cloning, place the required fonts here manually.

## Required Fonts

| File | Used for |
|---|---|
| `Archemy.otf` | Planetary glyphs in the HUD (☉ ☽ ♂ ☿ ♃ ♀ ♄), Ternary Order letters, alchemical UI elements |

## Fallback Behaviour

If a font file is missing, the game will log a warning and fall back to Raylib's built-in default font. Planetary glyphs will display as `?` until the font is placed here.

The fallback is handled in `src/core/fonts.h` via `LoadAlchemyFont()`.
