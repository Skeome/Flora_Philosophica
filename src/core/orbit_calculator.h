#ifndef FLORA_PHILOSOPHICA_CORE_ORBIT_H
#define FLORA_PHILOSOPHICA_CORE_ORBIT_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

// ─────────────────────────────────────────────────────────────────────────────
// PlanetaryOrbitCalculator
// GDExtension wrapper around an original clean-room VSOP87 implementation.
//
// Provides true geocentric ecliptic longitude AND latitude for the seven
// classical planets (+ Sun + Moon) for use in the main menu's "dance of the
// spheres" orbit animation. Latitude is what produces genuine apparent
// retrograde loops (Mercury/Venus/Mars/Jupiter/Saturn) rather than a flat
// longitude-only sweep.
//
// Usage from GDScript:
//   var calc = PlanetaryOrbitCalculator.new()
//   var pos = calc.get_geocentric_position("Mars", unix_timestamp)
//   # pos == { "lon": float (degrees, 0-360), "lat": float (degrees) }
//
//   var all = calc.get_all_geocentric_positions(unix_timestamp)
//   # all == { "Mars": {"lon":.., "lat":..}, "Venus": {...}, ... }
// ─────────────────────────────────────────────────────────────────────────────

namespace godot {

class PlanetaryOrbitCalculator : public RefCounted {
    GDCLASS(PlanetaryOrbitCalculator, RefCounted)

public:
    PlanetaryOrbitCalculator();
    ~PlanetaryOrbitCalculator();

    // Returns { "lon": degrees, "lat": degrees } for a single body.
    // observer_lat/observer_lon (degrees) are accepted for future parallax
    // correction; at planetary distances the effect is sub-arcsecond and
    // currently unused in the calculation, but the parameters are wired
    // through so real player location is available when needed.
    Dictionary get_geocentric_position(const String& planet_name, int64_t utc_timestamp,
                                        double observer_lat = 0.0, double observer_lon = 0.0) const;

    // Returns { planet_name: {"lon":.., "lat":..}, ... } for all seven bodies.
    Dictionary get_all_geocentric_positions(int64_t utc_timestamp,
                                             double observer_lat = 0.0, double observer_lon = 0.0) const;

    // Mean daily motion in degrees/day — used by GDScript only as a fallback
    // when the C++ calculator can't be reached. The real animation should
    // prefer get_geocentric_position() sampled across the fast-forward
    // window instead of extrapolating from mean motion, since mean motion
    // can't reproduce retrograde loops.
    static double get_mean_daily_motion(const String& planet_name);

protected:
    static void _bind_methods();

private:
    static double unix_to_julian_day(int64_t unix_ts);

    struct HeliocentricCoords {
        double L; // heliocentric ecliptic longitude, radians
        double B; // heliocentric ecliptic latitude, radians
        double R; // radius vector, AU
    };

    static HeliocentricCoords heliocentric_earth(double T);
    static HeliocentricCoords heliocentric_planet(int planet_index, double T);

    // Converts heliocentric L/B/R into rectangular ecliptic XYZ (AU)
    static void to_rectangular(const HeliocentricCoords& h, double& x, double& y, double& z);
};

} // namespace godot

#endif // FLORA_PHILOSOPHICA_CORE_ORBIT_H
