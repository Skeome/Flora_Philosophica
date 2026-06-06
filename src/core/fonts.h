#ifndef FLORA_PHILOSOPHICA_CORE_FONTS_H
#define FLORA_PHILOSOPHICA_CORE_FONTS_H

#include "raylib.h"
#include <cstdio>

namespace FloraPhilosophica {
namespace Core {

// ─────────────────────────────────────────────────────────────────────────────
// Font paths
// assets/fonts/ is listed in .gitignore and never committed to the repository.
// Place font files there manually after cloning.
//
// ARCHEMY_FONT_PATH — Archemy OTF, a proprietary alchemical symbol font.
// Used for: planetary glyphs in the HUD, Ternary Order letters, any UI
// element requiring authentic alchemical or astrological characters.
// ─────────────────────────────────────────────────────────────────────────────
static constexpr const char* ARCHEMY_FONT_PATH = "assets/fonts/Archemy.otf";

// ─────────────────────────────────────────────────────────────────────────────
// Unicode codepoints for the seven classical planets.
// These are the codepoints Archemy provides glyphs for.
// Passed to LoadFontEx so Raylib only rasterizes what we need.
// ─────────────────────────────────────────────────────────────────────────────
static constexpr int PLANETARY_CODEPOINTS[] = {
    0x2609,  // ☉ Sun
    0x263D,  // ☽ Moon
    0x2642,  // ♂ Mars
    0x263F,  // ☿ Mercury
    0x2643,  // ♃ Jupiter
    0x2640,  // ♀ Venus
    0x2644,  // ♄ Saturn
    0x0020,  // Space (always include for spacing)
};
static constexpr int PLANETARY_CODEPOINT_COUNT = 8;

// ─────────────────────────────────────────────────────────────────────────────
// LoadAlchemyFont
// Attempts to load Archemy at the given pixel size.
// If the font file is missing (e.g. fresh clone without assets), falls back
// to Raylib's built-in default font and logs a warning.
// Always returns a valid Font — never crashes on missing file.
// ─────────────────────────────────────────────────────────────────────────────
inline Font LoadAlchemyFont(int size = 24) {
    // Check if the font file exists before asking Raylib to load it.
    // Raylib will log its own error if the file is missing, but we want
    // a clean fallback rather than a broken glyph display.
    FILE* f = std::fopen(ARCHEMY_FONT_PATH, "rb");
    if (!f) {
        TraceLog(LOG_WARNING,
            "FONTS: Archemy.otf not found at '%s'. "
            "Place the font in assets/fonts/ (see .gitignore). "
            "Falling back to default font — planetary glyphs will show as '?'.",
            ARCHEMY_FONT_PATH);
        return GetFontDefault();
    }
    std::fclose(f);

    // Load only the codepoints we actually use.
    // This keeps the texture atlas small and load time fast.
    Font font = LoadFontEx(
        ARCHEMY_FONT_PATH,
        size,
        const_cast<int*>(PLANETARY_CODEPOINTS),
        PLANETARY_CODEPOINT_COUNT
    );

    TraceLog(LOG_INFO, "FONTS: Loaded Archemy.otf at %dpx", size);
    return font;
}

} // namespace Core
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_CORE_FONTS_H
