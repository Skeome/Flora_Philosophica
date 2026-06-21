#include "orbit_calculator.h"

// Clean-room VSOP87 truncated coefficients — see vsop87_terms.h for
// sourcing notes. No GPL or third-party code is used in this file.
#include "vsop87_terms.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>
#include <cstring>
#include <string>

namespace {
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;

    inline double radToDeg(double r) { return r * (180.0 / PI); }
    inline double degToRad(double d) { return d * (PI / 180.0); }

    template <size_t N>
    double sumSeries(const FloraPhilosophica::Core::VSOP87Terms::Term (&terms)[N], double T) {
        double sum = 0.0;
        for (size_t i = 0; i < N; ++i) {
            sum += terms[i].A * std::cos(terms[i].B + terms[i].C * T);
        }
        return sum;
    }

    // L = L0 + L1*T  (longitude, has a linear secular term)
    template <size_t N0, size_t N1>
    double sumLongitudeSeries(
            const FloraPhilosophica::Core::VSOP87Terms::Term (&l0)[N0],
            const FloraPhilosophica::Core::VSOP87Terms::Term (&l1)[N1],
            double T) {
        double L = sumSeries(l0, T) + sumSeries(l1, T) * T;
        L = std::fmod(L, TWO_PI);
        if (L < 0.0) L += TWO_PI;
        return L;
    }

    // R = R0 + R1*T  (radius, has a linear secular term for Earth specifically;
    // the truncated planet series here use R0 only, which is sufficient at
    // this precision level)
    template <size_t N0, size_t N1>
    double sumRadiusSeries(
            const FloraPhilosophica::Core::VSOP87Terms::Term (&r0)[N0],
            const FloraPhilosophica::Core::VSOP87Terms::Term (&r1)[N1],
            double T) {
        return sumSeries(r0, T) + sumSeries(r1, T) * T;
    }
}

namespace godot {

// ─────────────────────────────────────────────────────────────────────────────
// Mean daily motions (degrees/day) — classical values, public domain.
// Source: Jean Meeus, Astronomical Algorithms, Ch. 21.
// Used only as a GDScript-side fallback when the C++ class is unavailable.
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
        D_METHOD("get_geocentric_position", "planet_name", "utc_timestamp", "observer_lat", "observer_lon"),
        &PlanetaryOrbitCalculator::get_geocentric_position,
        DEFVAL(0.0), DEFVAL(0.0));
    ClassDB::bind_method(
        D_METHOD("get_all_geocentric_positions", "utc_timestamp", "observer_lat", "observer_lon"),
        &PlanetaryOrbitCalculator::get_all_geocentric_positions,
        DEFVAL(0.0), DEFVAL(0.0));
    ClassDB::bind_static_method(
        "PlanetaryOrbitCalculator",
        D_METHOD("get_mean_daily_motion", "planet_name"),
        &PlanetaryOrbitCalculator::get_mean_daily_motion);
}

// ── Helpers ──────────────────────────────────────────────────────────────────

double PlanetaryOrbitCalculator::unix_to_julian_day(int64_t unix_ts) {
    return 2440587.5 + static_cast<double>(unix_ts) / 86400.0;
}

PlanetaryOrbitCalculator::HeliocentricCoords
PlanetaryOrbitCalculator::heliocentric_earth(double T) {
    using namespace FloraPhilosophica::Core::VSOP87Terms;
    HeliocentricCoords h;
    h.L = sumLongitudeSeries(Earth_L0, Earth_L1, T);
    h.B = sumSeries(Earth_B0, T);
    h.R = sumRadiusSeries(Earth_R0, Earth_R1, T);
    return h;
}

PlanetaryOrbitCalculator::HeliocentricCoords
PlanetaryOrbitCalculator::heliocentric_planet(int planet_index, double T) {
    using namespace FloraPhilosophica::Core::VSOP87Terms;
    const char* name = PLANET_TABLE[planet_index].name;
    HeliocentricCoords h{0.0, 0.0, 1.0};

    if (std::strcmp(name, "Mercury") == 0) {
        h.L = sumLongitudeSeries(Mercury_L0, Mercury_L1, T);
        h.B = sumSeries(Mercury_B0, T);
        h.R = sumSeries(Mercury_R0, T);
    } else if (std::strcmp(name, "Venus") == 0) {
        h.L = sumLongitudeSeries(Venus_L0, Venus_L1, T);
        h.B = sumSeries(Venus_B0, T);
        h.R = sumSeries(Venus_R0, T);
    } else if (std::strcmp(name, "Mars") == 0) {
        h.L = sumLongitudeSeries(Mars_L0, Mars_L1, T);
        h.B = sumSeries(Mars_B0, T);
        h.R = sumSeries(Mars_R0, T);
    } else if (std::strcmp(name, "Jupiter") == 0) {
        h.L = sumLongitudeSeries(Jupiter_L0, Jupiter_L1, T);
        h.B = sumSeries(Jupiter_B0, T);
        h.R = sumSeries(Jupiter_R0, T);
    } else if (std::strcmp(name, "Saturn") == 0) {
        h.L = sumLongitudeSeries(Saturn_L0, Saturn_L1, T);
        h.B = sumSeries(Saturn_B0, T);
        h.R = sumSeries(Saturn_R0, T);
    }

    return h;
}

void PlanetaryOrbitCalculator::to_rectangular(const HeliocentricCoords& h, double& x, double& y, double& z) {
    x = h.R * std::cos(h.B) * std::cos(h.L);
    y = h.R * std::cos(h.B) * std::sin(h.L);
    z = h.R * std::sin(h.B);
}

// ── Public API ────────────────────────────────────────────────────────────────

Dictionary PlanetaryOrbitCalculator::get_geocentric_position(
        const String& planet_name, int64_t utc_timestamp,
        double observer_lat, double observer_lon) const {

    double jd = unix_to_julian_day(utc_timestamp);
    double T = (jd - 2451545.0) / 365250.0;

    Dictionary result;
    std::string name_str = planet_name.utf8().get_data();

    // ── Sun: geocentric longitude is Earth's heliocentric longitude + 180°.
    //    Latitude is ~0 by definition (the Sun always sits on the ecliptic
    //    as seen from Earth, to first order).
    if (name_str == "Sun") {
        HeliocentricCoords earth = heliocentric_earth(T);
        double sunLon = std::fmod(earth.L + PI, TWO_PI);
        result["lon"] = radToDeg(sunLon);
        result["lat"] = radToDeg(-earth.B); // Sun's apparent latitude mirrors Earth's tiny B
        return result;
    }

    // ── Moon: independent low-precision lunar approximation (NOT ELP2000).
    //    Mean longitude + two-term equation of centre, plus a mean-latitude
    //    term from the Moon's ~5.14° orbital inclination to the ecliptic.
    //    Decorative-grade only.
    if (name_str == "Moon") {
        double d = jd - 2451545.0;
        double Lp = 218.3164591 + 13.17639648 * d;
        double Mp = 134.9634114 + 13.06499295 * d;
        double D  = 297.8502042 + 12.19074912 * d;
        double F  = 93.2720993  + 13.22935024 * d; // argument of latitude

        double MpRad = degToRad(std::fmod(Mp, 360.0));
        double DRad  = degToRad(std::fmod(D, 360.0));
        double FRad  = degToRad(std::fmod(F, 360.0));

        double lonCorrection = 6.289 * std::sin(MpRad) + 1.274 * std::sin(2.0 * DRad - MpRad);
        double lon = std::fmod(Lp + lonCorrection, 360.0);
        if (lon < 0.0) lon += 360.0;

        // Dominant latitude term — Moon's orbital inclination ~5.13°
        double lat = 5.128 * std::sin(FRad);

        result["lon"] = lon;
        result["lat"] = lat;
        return result;
    }

    // ── Planets: full geocentric position via heliocentric rectangular
    //    subtraction. This is what produces genuine apparent retrograde
    //    loops — they emerge naturally from Earth and the planet moving
    //    at different angular rates around the Sun, not from any synthetic
    //    "loop" function.
    int planet_index = -1;
    for (int i = 0; i < PLANET_COUNT; ++i) {
        if (name_str == PLANET_TABLE[i].name) { planet_index = i; break; }
    }
    if (planet_index < 0) {
        UtilityFunctions::printerr("PlanetaryOrbitCalculator: unknown planet '", planet_name, "'");
        result["lon"] = 0.0;
        result["lat"] = 0.0;
        return result;
    }

    HeliocentricCoords earth  = heliocentric_earth(T);
    HeliocentricCoords planet = heliocentric_planet(planet_index, T);

    double ex, ey, ez, px, py, pz;
    to_rectangular(earth, ex, ey, ez);
    to_rectangular(planet, px, py, pz);

    double gx = px - ex;
    double gy = py - ey;
    double gz = pz - ez;

    double dist = std::sqrt(gx * gx + gy * gy);
    double lonRad = std::atan2(gy, gx);
    double latRad = std::atan2(gz, dist);

    double lonDeg = std::fmod(radToDeg(lonRad) + 360.0, 360.0);
    double latDeg = radToDeg(latRad);

    result["lon"] = lonDeg;
    result["lat"] = latDeg;
    return result;
}

Dictionary PlanetaryOrbitCalculator::get_all_geocentric_positions(
        int64_t utc_timestamp, double observer_lat, double observer_lon) const {

    Dictionary result;
    for (int i = 0; i < PLANET_COUNT; ++i) {
        String name = String(PLANET_TABLE[i].name);
        result[name] = get_geocentric_position(name, utc_timestamp, observer_lat, observer_lon);
    }
    return result;
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
