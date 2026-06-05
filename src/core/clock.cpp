#include "clock.h"
#include <cmath>
#include <string>

// Anonymous namespace for mathematical constants and helper functions.
// In C++, variables and functions declared in an anonymous namespace are 
// local to the translation unit (.cpp file), avoiding symbol collisions.
namespace {
    // Standard definition of Pi used for trigonometric calculations.
    constexpr double PI = 3.14159265358979323846;

    // Helper function to convert degrees to radians.
    // C++ math functions (<cmath>) like std::sin and std::cos require arguments in radians.
    inline double degToRad(double angle) {
        return angle * (PI / 180.0);
    }

    // Helper function to convert radians to degrees.
    // Standard outputs of arcsin (std::asin), arccos (std::acos), and arctan (std::atan2) 
    // are in radians; we convert them back to degrees for astronomical formulas.
    inline double radToDeg(double angle) {
        return angle * (180.0 / PI);
    }
}

namespace FloraPhilosophica {
namespace Core {

// Constructor
AstrologicalClock::AstrologicalClock() {}

// AstrologicalClock::CalculateSolarLimits
// Computes sunrise and sunset times (in minutes from UTC midnight) for a given Julian Date 
// utilizing the NOAA Solar Position Algorithm.
// For references, see: https://gml.noaa.gov/grad/solcalc/solareqns.PDF
AstrologicalClock::SolarTimes AstrologicalClock::CalculateSolarLimits(double latitude, double longitude, double julianDate) {
    SolarTimes result;
    result.isPolar = false;

    // 1. Calculate Julian Century (JC) relative to the J2000.0 Epoch (noon on Jan 1, 2000 UTC).
    // NOAA formulas are calibrated to JC, where 36525 represents the number of days in a Julian century.
    double JC = (julianDate - 2451545.0) / 36525.0;

    // 2. Calculate the Geometric Mean Longitude of the Sun (L0) in degrees.
    // This represents the average position of the Sun along the ecliptic if Earth's orbit were perfectly circular.
    double L0 = 280.46646 + JC * (36000.76983 + JC * 0.0003032);
    // Keep angle in range [0, 360) using fmod (floating-point modulo)
    L0 = std::fmod(L0, 360.0);
    if (L0 < 0.0) {
        L0 += 360.0;
    }

    // 3. Calculate the Geometric Mean Anomaly of the Sun (M) in degrees.
    // This measures the angle of the Earth relative to its perihelion (closest point to the Sun).
    double M = 357.52911 + JC * (35999.05029 - JC * 0.0001537);

    // 4. Calculate the Eccentricity of Earth's Orbit (e).
    // The eccentricity describes how elliptical (stretched) Earth's orbit is.
    double e = 0.016708634 - JC * (0.000042037 + JC * 0.0000001267);

    // 5. Calculate the Sun's Equation of the Center (C).
    // This represents the angular difference between the actual elliptical position of the Sun 
    // and its mean circular position.
    double M_rad = degToRad(M);
    double C = std::sin(M_rad) * (1.914602 - JC * (0.004817 + JC * 0.000014))
             + std::sin(2.0 * M_rad) * (0.019993 - JC * 0.000101)
             + std::sin(3.0 * M_rad) * 0.000289;

    // 6. Calculate the Sun's True Longitude (trueLong) in degrees.
    // The true position of the Sun along the ecliptic.
    double trueLong = L0 + C;

    // 7. Calculate the Sun's Apparent Longitude (apparentLong) in degrees.
    // True longitude corrected for aberration (light travel time) and nutation (Earth axis wobble).
    double apparentLong = trueLong - 0.00569 - 0.00478 * std::sin(degToRad(125.04 - 1934.136 * JC));

    // 8. Calculate Mean Obliquity of the Ecliptic (epsilon0) in degrees.
    // The average tilt angle of Earth's rotational axis relative to its orbital plane.
    double epsilon0 = 23.0 + (26.0 + (21.448 - JC * (46.815 + JC * (0.00059 - JC * 0.001813))) / 60.0) / 60.0;

    // 9. Calculate Corrected Obliquity (epsilon) in degrees.
    // Obliquity corrected for nutation.
    double epsilon = epsilon0 + 0.00256 * std::cos(degToRad(125.04 - 1934.136 * JC));

    // 10. Calculate the Sun's Declination (declination) in degrees.
    // The angle of the Sun relative to Earth's celestial equator (North/South angle).
    double declination = radToDeg(std::asin(std::sin(degToRad(epsilon)) * std::sin(degToRad(apparentLong))));

    // 11. Calculate the Equation of Time (eqTime) in minutes.
    // The difference between apparent solar time (sundial) and mean solar time (clocks).
    // It accounts for Earth's axial tilt and orbital eccentricity.
    double y = std::tan(degToRad(epsilon / 2.0));
    y = y * y;
    double L0_rad = degToRad(L0);
    double eqTime = 4.0 * radToDeg(
        y * std::sin(2.0 * L0_rad)
        - 2.0 * e * std::sin(M_rad)
        + 4.0 * e * y * std::sin(M_rad) * std::cos(2.0 * L0_rad)
        - 0.5 * y * y * std::sin(4.0 * L0_rad)
        - 1.25 * e * e * std::sin(2.0 * M_rad)
    );

    // 12. Calculate the Hour Angle at Sunrise/Sunset (HA0) in degrees.
    // The Hour Angle is the angular distance (measured along the celestial equator) 
    // from the observer's meridian to the Sun.
    // Standard refraction correction uses 90.833 degrees (90 degrees for horizon + 50 arcminutes refraction/semi-diameter).
    double cosHA = std::cos(degToRad(90.833)) / (std::cos(degToRad(latitude)) * std::cos(degToRad(declination)))
                 - std::tan(degToRad(latitude)) * std::tan(degToRad(declination));

    // Check for extreme polar conditions:
    // If cosHA > 1, the Sun never rises (polar night).
    // If cosHA < -1, the Sun never sets (polar day).
    if (cosHA > 1.0 || cosHA < -1.0) {
        result.isPolar = true;
        result.sunriseUTC = 0.0;
        result.sunsetUTC = 0.0;
        return result;
    }

    // Solve for HA0 in degrees
    double HA0 = radToDeg(std::acos(cosHA));

    // 13. Calculate Solar Noon UTC (in minutes since UTC midnight).
    // 720.0 represents the midday point (12 hours * 60 minutes).
    // Longitude is subtracted/added to shift the noon point based on geography (4.0 minutes per degree).
    // eqTime is subtracted to account for Earth's velocity changes.
    double noonUTC = 720.0 - 4.0 * longitude - eqTime;

    // 14. Calculate Sunrise and Sunset UTC (in minutes since UTC midnight).
    // Since 15 degrees of rotation = 1 hour, each degree = 4 minutes. 
    // Hence, the Hour Angle (HA0) is multiplied by 4.0.
    result.sunriseUTC = noonUTC - 4.0 * HA0;
    result.sunsetUTC = noonUTC + 4.0 * HA0;

    return result;
}

// AstrologicalClock::GetDayLimits
// Helper that translates the relative solar times (in minutes from midnight) 
// into absolute Unix timestamps for a given civil calendar day.
AstrologicalClock::DayLimits AstrologicalClock::GetDayLimits(double latitude, double longitude, long long utcTimestamp, int dayOffset) {
    DayLimits limits;
    limits.isPolar = false;

    // 1. Calculate civil midnight of today (seconds since Jan 1, 1970 UTC).
    // Floor dividing the timestamp by 86400.0 (number of seconds in a day) gives 
    // the day boundary representation.
    double rawDays = static_cast<double>(utcTimestamp) / 86400.0;
    long long midnightToday = static_cast<long long>(std::floor(rawDays)) * 86400;

    // 2. Adjust midnight by the requested dayOffset (-1 for yesterday, 0 for today, 1 for tomorrow)
    long long targetMidnight = midnightToday + dayOffset * 86400;

    // 3. Find the Julian Date corresponding to Solar Noon (12:00:00 UTC) on the target day.
    // 2440588.0 is the Julian Date at noon UTC on Unix Epoch (Jan 1, 1970).
    double julianNoon = (static_cast<double>(targetMidnight) / 86400.0) + 2440588.0;

    // 4. Calculate solar limits relative to the noon coordinates
    SolarTimes times = CalculateSolarLimits(latitude, longitude, julianNoon);

    if (times.isPolar) {
        limits.isPolar = true;
        limits.sunriseUnix = 0;
        limits.sunsetUnix = 0;
    } else {
        // Convert local minutes-from-midnight boundaries to absolute Unix timestamps (seconds)
        limits.sunriseUnix = targetMidnight + static_cast<long long>(times.sunriseUTC * 60.0);
        limits.sunsetUnix = targetMidnight + static_cast<long long>(times.sunsetUTC * 60.0);
    }

    return limits;
}

// AstrologicalClock::CalculatePlanetaryHour
// Maps any given UTC timestamp and observer coordinate to the correct planetary hour state.
PlanetaryHourInfo AstrologicalClock::CalculatePlanetaryHour(double latitude, double longitude, long long utcTimestamp) {
    // 1. Determine sunrise/sunset limits for yesterday, today, and tomorrow.
    // This allows us to handle transitions across midnight boundaries seamlessly.
    DayLimits yesterday = GetDayLimits(latitude, longitude, utcTimestamp, -1);
    DayLimits today = GetDayLimits(latitude, longitude, utcTimestamp, 0);
    DayLimits tomorrow = GetDayLimits(latitude, longitude, utcTimestamp, 1);

    long long activeStart = 0;
    long long activeEnd = 0;
    int hourOffset = 0;         // 0 for daytime hours (0-11), 12 for nighttime hours (12-23)
    long long targetMidnight = 0; // Midnight timestamp of the active astrological day
    bool isPolarActive = false;

    double rawDays = static_cast<double>(utcTimestamp) / 86400.0;
    long long midnightToday = static_cast<long long>(std::floor(rawDays)) * 86400;

    // 2. Determine which day/night segment the current timestamp belongs to.
    if (today.isPolar) {
        // Polar region fallback (Polar Day or Polar Night):
        // Since sunrise/sunset cannot be computed, default to 24 equal segments of 1 hour 
        // starting at midnight today.
        isPolarActive = true;
        activeStart = midnightToday;
        activeEnd = midnightToday + 86400;
        targetMidnight = midnightToday;
    } else {
        // Normal region: Check standard boundaries.
        if (utcTimestamp >= today.sunriseUnix && utcTimestamp < today.sunsetUnix) {
            // Case A: Current time is after today's sunrise and before today's sunset (Daytime).
            activeStart = today.sunriseUnix;
            activeEnd = today.sunsetUnix;
            hourOffset = 0;
            targetMidnight = midnightToday;
        } 
        else if (utcTimestamp >= today.sunsetUnix && utcTimestamp < tomorrow.sunriseUnix) {
            // Case B: Current time is after today's sunset and before tomorrow's sunrise (Nighttime).
            activeStart = today.sunsetUnix;
            activeEnd = tomorrow.sunriseUnix;
            hourOffset = 12;
            targetMidnight = midnightToday;
        } 
        else if (utcTimestamp < today.sunriseUnix) {
            // Case C: Current time is before today's sunrise. 
            // In astrological scheduling, this belongs to yesterday's night segment.
            if (yesterday.isPolar) {
                // Extreme polar case safety check
                isPolarActive = true;
                long long midnightYesterday = midnightToday - 86400;
                activeStart = midnightYesterday;
                activeEnd = midnightYesterday + 86400;
                targetMidnight = midnightYesterday;
            } else {
                activeStart = yesterday.sunsetUnix;
                activeEnd = today.sunriseUnix;
                hourOffset = 12;
                targetMidnight = midnightToday - 86400;
            }
        } 
        else {
            // Case D: Current time is past tomorrow's sunrise (safety fallback if bounds shift)
            activeStart = tomorrow.sunriseUnix;
            activeEnd = tomorrow.sunsetUnix;
            hourOffset = 0;
            targetMidnight = midnightToday + 86400;
        }
    }

    // 3. Compute elapsed time and division widths.
    double totalDuration = static_cast<double>(activeEnd - activeStart);
    double elapsed = static_cast<double>(utcTimestamp - activeStart);

    int hourIndex = 0;
    double minutesRemaining = 0.0;

    if (isPolarActive) {
        // Polar fallback math: 24 equal segments of 1 hour (3600 seconds)
        double segmentDuration = 3600.0;
        hourIndex = static_cast<int>(std::floor(elapsed / segmentDuration));
        
        // Defensive bounds checks
        if (hourIndex < 0) hourIndex = 0;
        if (hourIndex > 23) hourIndex = 23;
        
        minutesRemaining = (segmentDuration - std::fmod(elapsed, segmentDuration)) / 60.0;
    } else {
        // Astrological math: Divide day or night duration into 12 unequal segments.
        double segmentDuration = totalDuration / 12.0;
        int relativeHour = static_cast<int>(std::floor(elapsed / segmentDuration));
        
        // Defensive bounds checks
        if (relativeHour < 0) relativeHour = 0;
        if (relativeHour > 11) relativeHour = 11;

        hourIndex = hourOffset + relativeHour;
        minutesRemaining = (segmentDuration - std::fmod(elapsed, segmentDuration)) / 60.0;
    }

    // 4. Calculate Julian Date at noon of the active astrological day.
    // This gives us a fixed point to compute its weekday.
    double julianNoon = (static_cast<double>(targetMidnight) / 86400.0) + 2440588.0;
    long long JDN = static_cast<long long>(std::floor(julianNoon));

    // 5. Determine the weekday index of the active astrological day.
    // Julian Day 0 (noon Jan 1, 4713 BC) was a Monday.
    // Therefore, (JDN + 1) % 7 calculates weekday index:
    // 0 = Sunday, 1 = Monday, 2 = Tuesday, 3 = Wednesday, 4 = Thursday, 5 = Friday, 6 = Saturday
    int weekdayIndex = static_cast<int>((JDN + 1) % 7);

    // 6. Map weekday index to its corresponding ruling planet of the first hour.
    // Every weekday has a planetary ruler traditionally assigned to its sunrise:
    Planet dayRuler = Planet::Sun;
    switch (weekdayIndex) {
        case 0: dayRuler = Planet::Sun;     break; // Sunday is ruled by the Sun
        case 1: dayRuler = Planet::Moon;    break; // Monday is ruled by the Moon
        case 2: dayRuler = Planet::Mars;    break; // Tuesday is ruled by Mars
        case 3: dayRuler = Planet::Mercury; break; // Wednesday is ruled by Mercury
        case 4: dayRuler = Planet::Jupiter; break; // Thursday is ruled by Jupiter
        case 5: dayRuler = Planet::Venus;   break; // Friday is ruled by Venus
        case 6: dayRuler = Planet::Saturn;  break; // Saturday is ruled by Saturn
    }

    // 7. Calculate ruling planet of the current hour using the Chaldean sequence.
    // In Chaldean Order, the planet index changes by 1 every hour.
    // Since our Planet enum index matches the Chaldean sequence order:
    // Saturn (0) -> Jupiter (1) -> Mars (2) -> Sun (3) -> Venus (4) -> Mercury (5) -> Moon (6),
    // we can find the ruling planet via simple math: (DayRulerIndex + HourIndex) % 7.
    int planetIndex = (static_cast<int>(dayRuler) + hourIndex) % 7;
    Planet rulingPlanet = static_cast<Planet>(planetIndex);

    // 8. Calculate the absolute Unix timestamps for the start and end of the current hour.
    // These are used by the UI countdown timer and the Consecration mechanic.
    double segmentDurationFinal = isPolarActive
        ? 3600.0
        : (totalDuration / 12.0);

    int relativeHourFinal = isPolarActive
        ? hourIndex
        : (hourIndex - hourOffset);

    long long hourStartUtc = activeStart + static_cast<long long>(relativeHourFinal * segmentDurationFinal);
    long long hourEndUtc   = activeStart + static_cast<long long>((relativeHourFinal + 1) * segmentDurationFinal);

    // 9. Construct response payload
    PlanetaryHourInfo info;
    info.rulingPlanet     = rulingPlanet;
    info.dayRuler         = dayRuler;       // Exposed so harvest quality can check day+hour match
    info.hourIndex        = hourIndex;
    info.minutesRemaining = minutesRemaining;
    info.planetName       = GetPlanetName(rulingPlanet);
    info.hourStartUtc     = hourStartUtc;
    info.hourEndUtc       = hourEndUtc;

    return info;
}

// AstrologicalClock::GetPlanetName
// Returns a descriptive string representation of the planet enum.
std::string AstrologicalClock::GetPlanetName(Planet planet) {
    switch (planet) {
        case Planet::Saturn:  return "Saturn";
        case Planet::Jupiter: return "Jupiter";
        case Planet::Mars:    return "Mars";
        case Planet::Sun:     return "Sun";
        case Planet::Venus:   return "Venus";
        case Planet::Mercury: return "Mercury";
        case Planet::Moon:    return "Moon";
    }
    return "Unknown";
}

} // namespace Core
} // namespace FloraPhilosophica