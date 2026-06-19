#include "orbit_calculator.h"

// Clean-room VSOP87 truncated coefficients — see vsop87_terms.h for
// sourcing notes. No GPL or third-party code is used in this file.
#include "vsop87_terms.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>
#include <cstring>

namespace {
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;

    inline double radToDeg(double r) { return r * (180.0 / PI); }
    inline double degToRad(double d) { return d * (PI / 180.0); }

    // Sum a VSOP87 term series at Julian millennium T
    template <size_t N>
    double sumSeries(const FloraPhilosophica::Core::VSOP87Terms::Term (&terms)[N], double T) {
        double sum = 0.0;
        for (size_t i = 0; i < N; ++i) {
            sum += terms[i].A * std::cos(terms[i].B + terms[i].C * T);
        }
        return sum;
    }

    // Heliocentric longitude for a body using its L0 + L1*T series
    template <size_t N0, size_t N1>
    double heliocentricLongitude(
            const FloraPhilosophica::Core::VSOP87Terms::Term (&l0)[N0],
            const FloraPhilosophica::Core::VSOP87Terms::Term (&l1)[N1],
            double T) {
        double L0 = sumSeries(l0, T);
        double L1 = sumSeries(l1, T);
        double L = L0 + L1 * T;
        // Normalise to [0, 2π)
        L = std::fmod(L, TWO_PI);
        if (L < 0.0) L += TWO_PI;
        return L;
    }

    // Approximate heliocentric radius for the inner-loop geocentric correction.
    // Using mean semi-major axis is sufficient for a decorative animation —
    // full radius series is not transcribed here since only longitude is used.
    double meanDistanceAU(const char* name) {
        if (std::strcmp(name, "Mercury") == 0) return 0.3871;
        if (std::strcmp(name, "Venus")   == 0) return 0.7233;
        if (std::strcmp(name, "Earth")   == 0) return 1.0000;
        if (std::strcmp(name, "Mars")    == 0) return 1.5237;
        if (std::strcmp(name, "Jupiter") == 0) return 5.2026;
        if (std::strcmp(name, "Saturn")  == 0) return 9.5549;
        return 1.0;
    }
}

namespace godot {

// ─────────────────────────────────────────────────────────────────────────────
// Mean daily motions (degrees/day) — classical values, public domain.
// Source: Jean Meeus, Astronomical Algorithms, Ch. 21 (orbital elements table).
// ─────────────────────────────────────────────────────────────────────────────
static const struct {
    const char* name;
    double      daily_motion;
} PLANET_TABLE[] = {
    { "Sun",     0.9856  },
    { "Moon",   13.1764  },
    { "Mercury", 4.0923  },
    { "Venus",   1.6021  },
    { "Mars",    0.5240  },
    { "Jupiter", 0.0831  },
    { "Saturn",  0.0335  },
};
static constexpr int PLANET_COUNT = 7;

PlanetaryOrbitCalculator::PlanetaryOrbitCalculator() {}
PlanetaryOrbitCalculator::~PlanetaryOrbitCalculator() {}

void PlanetaryOrbitCalculator::_bind_methods() {
    ClassDB::bind_method(
        D_METHOD("get_all_positions", "utc_timestamp"),
        &PlanetaryOrbitCalculator::get_all_positions);
    ClassDB::bind_method(
        D_METHOD("get_planet_longitude", "planet_name", "utc_timestamp"),
        &PlanetaryOrbitCalculator::get_planet_longitude);
    ClassDB::bind_static_method(
        "PlanetaryOrbitCalculator",
        D_METHOD("get_mean_daily_motion", "planet_name"),
        &PlanetaryOrbitCalculator::get_mean_daily_motion);
}

// ── Helpers ──────────────────────────────────────────────────────────────────

double PlanetaryOrbitCalculator::unix_to_julian_day(int64_t unix_ts) {
    // Unix epoch (1970-01-01 00:00:00 UTC) = JD 2440587.5
    return 2440587.5 + static_cast<double>(unix_ts) / 86400.0;
}

double PlanetaryOrbitCalculator::compute_longitude(
        int planet_index, double julian_day) const {

    using namespace FloraPhilosophica::Core::VSOP87Terms;

    // Julian millennia from J2000.0 — the VSOP87 time argument
    double T = (julian_day - 2451545.0) / 365250.0;

    const char* name = PLANET_TABLE[planet_index].name;

    // ── Sun: geocentric longitude of the Sun is Earth's heliocentric
    //    longitude + 180° (Earth and Sun are always opposite as seen
    //    from each other along the same line).
    if (std::strcmp(name, "Sun") == 0) {
        double earthLon = heliocentricLongitude(Earth_L0, Earth_L1, T);
        double sunLon = std::fmod(earthLon + PI, TWO_PI); // +180°
        return radToDeg(sunLon);
    }

    // ── Moon: independent low-precision lunar longitude approximation.
    //    This is NOT ELP2000 — it is a simple two-term mean-longitude
    //    + equation-of-centre model (Meeus Ch. 47 introduction, simplified
    //    mean elements), sufficient for decorative animation only.
    if (std::strcmp(name, "Moon") == 0) {
        // Days since J2000.0
        double d = julian_day - 2451545.0;

        // Mean longitude of the Moon (degrees), public-domain mean element
        double Lp = 218.3164591 + 13.17639648 * d;

        // Mean anomaly of the Moon (degrees)
        double Mp = 134.9634114 + 13.06499295 * d;

        // Mean elongation of the Moon from the Sun (degrees)
        double D = 297.8502042 + 12.19074912 * d;

        double MpRad = degToRad(std::fmod(Mp, 360.0));
        double DRad  = degToRad(std::fmod(D, 360.0));

        // Two dominant equation-of-centre terms (simplified)
        double correction = 6.289 * std::sin(MpRad) + 1.274 * std::sin(2.0 * DRad - MpRad);

        double lon = std::fmod(Lp + correction, 360.0);
        if (lon < 0.0) lon += 360.0;
        return lon;
    }

    // ── Inner/outer planets: geocentric longitude via heliocentric
    //    rectangular subtraction (Earth as origin).
    double earthLon = heliocentricLongitude(Earth_L0, Earth_L1, T);
    double earthR   = meanDistanceAU("Earth");
    double earthX   = earthR * std::cos(earthLon);
    double earthY   = earthR * std::sin(earthLon);

    double planetLon = 0.0;
    double planetR   = meanDistanceAU(name);

    if (std::strcmp(name, "Mercury") == 0) {
        planetLon = heliocentricLongitude(Mercury_L0, Mercury_L1, T);
    } else if (std::strcmp(name, "Venus") == 0) {
        planetLon = heliocentricLongitude(Venus_L0, Venus_L1, T);
    } else if (std::strcmp(name, "Mars") == 0) {
        planetLon = heliocentricLongitude(Mars_L0, Mars_L1, T);
    } else if (std::strcmp(name, "Jupiter") == 0) {
        planetLon = heliocentricLongitude(Jupiter_L0, Jupiter_L1, T);
    } else if (std::strcmp(name, "Saturn") == 0) {
        planetLon = heliocentricLongitude(Saturn_L0, Saturn_L1, T);
    }

    double planetX = planetR * std::cos(planetLon);
    double planetY = planetR * std::sin(planetLon);

    double geoX = planetX - earthX;
    double geoY = planetY - earthY;

    double geoLonDeg = radToDeg(std::atan2(geoY, geoX));
    geoLonDeg = std::fmod(geoLonDeg + 360.0, 360.0);
    return geoLonDeg;
}

// ── Public API ────────────────────────────────────────────────────────────────

Dictionary PlanetaryOrbitCalculator::get_all_positions(int64_t utc_timestamp) const {
    double jd = unix_to_julian_day(utc_timestamp);
    Dictionary result;

    for (int i = 0; i < PLANET_COUNT; ++i) {
        double lon = compute_longitude(i, jd);
        result[String(PLANET_TABLE[i].name)] = lon;
    }

    return result;
}

double PlanetaryOrbitCalculator::get_planet_longitude(
        const String& planet_name, int64_t utc_timestamp) const {

    double jd = unix_to_julian_day(utc_timestamp);

    for (int i = 0; i < PLANET_COUNT; ++i) {
        if (planet_name == PLANET_TABLE[i].name) {
            return compute_longitude(i, jd);
        }
    }

    UtilityFunctions::printerr(
        "PlanetaryOrbitCalculator: unknown planet '", planet_name, "'");
    return 0.0;
}

double PlanetaryOrbitCalculator::get_mean_daily_motion(const String& planet_name) {
    for (int i = 0; i < PLANET_COUNT; ++i) {
        if (planet_name == PLANET_TABLE[i].name) {
            return PLANET_TABLE[i].daily_motion;
        }
    }
    return 0.0;
}

} // namespace godot
