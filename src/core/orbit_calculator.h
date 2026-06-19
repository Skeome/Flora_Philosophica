#ifndef FLORA_PHILOSOPHICA_CORE_ORBIT_H
#define FLORA_PHILOSOPHICA_CORE_ORBIT_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

// ─────────────────────────────────────────────────────────────────────────────
// PlanetaryOrbitCalculator
// GDExtension wrapper around the Ephemeris/VSOP87 library.
//
// Provides real geocentric ecliptic longitudes for the seven classical planets
// (+ Sun + Moon) for use in the main menu orbit animation and any future
// astrological chart features.
//
// Called once at menu startup:
//   var calc = PlanetaryOrbitCalculator.new()
//   var positions = calc.get_all_positions(unix_timestamp)
//   # returns Dictionary: { "Sun": float, "Moon": float, "Mars": float, ... }
//   # values are geocentric ecliptic longitude in degrees (0–360)
//
// The GDScript orbit shader then uses these as starting angles and advances
// each planet at its mean daily motion each frame.
// ─────────────────────────────────────────────────────────────────────────────

namespace godot {

class PlanetaryOrbitCalculator : public RefCounted {
    GDCLASS(PlanetaryOrbitCalculator, RefCounted)

public:
    PlanetaryOrbitCalculator();
    ~PlanetaryOrbitCalculator();

    // Returns a Dictionary of { planet_name: ecliptic_longitude_degrees }
    // for the given UTC Unix timestamp.
    // Planets: Sun, Moon, Mercury, Venus, Mars, Jupiter, Saturn
    Dictionary get_all_positions(int64_t utc_timestamp) const;

    // Returns just one planet's ecliptic longitude.
    double get_planet_longitude(const String& planet_name, int64_t utc_timestamp) const;

    // Mean daily motion in degrees/day for each classical planet.
    // Used by GDScript to advance positions each frame without recalculating.
    static double get_mean_daily_motion(const String& planet_name);

protected:
    static void _bind_methods();

private:
    // Convert Unix timestamp to Julian Day number
    static double unix_to_julian_day(int64_t unix_ts);

    // Internal computation using VSOP87 via Ephemeris
    // Returns geocentric ecliptic longitude in degrees [0, 360)
    double compute_longitude(int planet_index, double julian_day) const;
};

} // namespace godot

#endif // FLORA_PHILOSOPHICA_CORE_ORBIT_H
