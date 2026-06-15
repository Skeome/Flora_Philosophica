#include "clock.h"
#include <godot_cpp/core/class_db.hpp>
#include <cmath>

namespace {
    constexpr double PI = 3.14159265358979323846;

    inline double degToRad(double angle) {
        return angle * (PI / 180.0);
    }

    inline double radToDeg(double angle) {
        return angle * (180.0 / PI);
    }
}

namespace godot {

PlanetaryHourCalculator::PlanetaryHourCalculator() {}
PlanetaryHourCalculator::~PlanetaryHourCalculator() {}

void PlanetaryHourCalculator::_bind_methods() {
    ClassDB::bind_method(D_METHOD("calculate_planetary_hour", "latitude", "longitude", "utc_timestamp"), &PlanetaryHourCalculator::calculate_planetary_hour);
    ClassDB::bind_static_method("PlanetaryHourCalculator", D_METHOD("get_planet_name", "planet"), &PlanetaryHourCalculator::get_planet_name);

    BIND_ENUM_CONSTANT(SATURN);
    BIND_ENUM_CONSTANT(JUPITER);
    BIND_ENUM_CONSTANT(MARS);
    BIND_ENUM_CONSTANT(SUN);
    BIND_ENUM_CONSTANT(VENUS);
    BIND_ENUM_CONSTANT(MERCURY);
    BIND_ENUM_CONSTANT(MOON);
}

PlanetaryHourCalculator::SolarTimes PlanetaryHourCalculator::calculate_solar_limits(double latitude, double longitude, double julianDate) {
    SolarTimes result;
    result.isPolar = false;

    double JC = (julianDate - 2451545.0) / 36525.0;

    double L0 = 280.46646 + JC * (36000.76983 + JC * 0.0003032);
    L0 = std::fmod(L0, 360.0);
    if (L0 < 0.0) {
        L0 += 360.0;
    }

    double M = 357.52911 + JC * (35999.05029 - JC * 0.0001537);
    double e = 0.016708634 - JC * (0.000042037 + JC * 0.0000001267);

    double M_rad = degToRad(M);
    double C = std::sin(M_rad) * (1.914602 - JC * (0.004817 + JC * 0.000014))
             + std::sin(2.0 * M_rad) * (0.019993 - JC * 0.000101)
             + std::sin(3.0 * M_rad) * 0.000289;

    double trueLong = L0 + C;
    double apparentLong = trueLong - 0.00569 - 0.00478 * std::sin(degToRad(125.04 - 1934.136 * JC));
    double epsilon0 = 23.0 + (26.0 + (21.448 - JC * (46.815 + JC * (0.00059 - JC * 0.001813))) / 60.0) / 60.0;
    double epsilon = epsilon0 + 0.00256 * std::cos(degToRad(125.04 - 1934.136 * JC));

    double declination = radToDeg(std::asin(std::sin(degToRad(epsilon)) * std::sin(degToRad(apparentLong))));

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

    double cosHA = std::cos(degToRad(90.833)) / (std::cos(degToRad(latitude)) * std::cos(degToRad(declination)))
                 - std::tan(degToRad(latitude)) * std::tan(degToRad(declination));

    if (cosHA > 1.0 || cosHA < -1.0) {
        result.isPolar = true;
        result.sunriseUTC = 0.0;
        result.sunsetUTC = 0.0;
        return result;
    }

    double HA0 = radToDeg(std::acos(cosHA));
    double noonUTC = 720.0 - 4.0 * longitude - eqTime;

    result.sunriseUTC = noonUTC - 4.0 * HA0;
    result.sunsetUTC = noonUTC + 4.0 * HA0;

    return result;
}

PlanetaryHourCalculator::DayLimits PlanetaryHourCalculator::get_day_limits(double latitude, double longitude, int64_t utcTimestamp, int dayOffset) {
    DayLimits limits;
    limits.isPolar = false;

    double rawDays = static_cast<double>(utcTimestamp) / 86400.0;
    int64_t midnightToday = static_cast<int64_t>(std::floor(rawDays)) * 86400;
    int64_t targetMidnight = midnightToday + dayOffset * 86400;
    double julianNoon = (static_cast<double>(targetMidnight) / 86400.0) + 2440588.0;

    SolarTimes times = calculate_solar_limits(latitude, longitude, julianNoon);

    if (times.isPolar) {
        limits.isPolar = true;
        limits.sunriseUnix = 0;
        limits.sunsetUnix = 0;
    } else {
        limits.sunriseUnix = targetMidnight + static_cast<int64_t>(times.sunriseUTC * 60.0);
        limits.sunsetUnix = targetMidnight + static_cast<int64_t>(times.sunsetUTC * 60.0);
    }

    return limits;
}

Dictionary PlanetaryHourCalculator::calculate_planetary_hour(double latitude, double longitude, int64_t utc_timestamp) {
    DayLimits yesterday = get_day_limits(latitude, longitude, utc_timestamp, -1);
    DayLimits today = get_day_limits(latitude, longitude, utc_timestamp, 0);
    DayLimits tomorrow = get_day_limits(latitude, longitude, utc_timestamp, 1);

    int64_t activeStart = 0;
    int64_t activeEnd = 0;
    int hourOffset = 0;
    int64_t targetMidnight = 0;
    bool isPolarActive = false;

    double rawDays = static_cast<double>(utc_timestamp) / 86400.0;
    int64_t midnightToday = static_cast<int64_t>(std::floor(rawDays)) * 86400;

    if (today.isPolar) {
        isPolarActive = true;
        activeStart = midnightToday;
        activeEnd = midnightToday + 86400;
        targetMidnight = midnightToday;
    } else {
        if (utc_timestamp >= today.sunriseUnix && utc_timestamp < today.sunsetUnix) {
            activeStart = today.sunriseUnix;
            activeEnd = today.sunsetUnix;
            hourOffset = 0;
            targetMidnight = midnightToday;
        } 
        else if (utc_timestamp >= today.sunsetUnix && utc_timestamp < tomorrow.sunriseUnix) {
            activeStart = today.sunsetUnix;
            activeEnd = tomorrow.sunriseUnix;
            hourOffset = 12;
            targetMidnight = midnightToday;
        } 
        else if (utc_timestamp < today.sunriseUnix) {
            if (yesterday.isPolar) {
                isPolarActive = true;
                int64_t midnightYesterday = midnightToday - 86400;
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
            activeStart = tomorrow.sunriseUnix;
            activeEnd = tomorrow.sunsetUnix;
            hourOffset = 0;
            targetMidnight = midnightToday + 86400;
        }
    }

    double totalDuration = static_cast<double>(activeEnd - activeStart);
    double elapsed = static_cast<double>(utc_timestamp - activeStart);

    int hourIndex = 0;
    double minutesRemaining = 0.0;

    if (isPolarActive) {
        double segmentDuration = 3600.0;
        hourIndex = static_cast<int>(std::floor(elapsed / segmentDuration));
        if (hourIndex < 0) hourIndex = 0;
        if (hourIndex > 23) hourIndex = 23;
        minutesRemaining = (segmentDuration - std::fmod(elapsed, segmentDuration)) / 60.0;
    } else {
        double segmentDuration = totalDuration / 12.0;
        int relativeHour = static_cast<int>(std::floor(elapsed / segmentDuration));
        if (relativeHour < 0) relativeHour = 0;
        if (relativeHour > 11) relativeHour = 11;
        hourIndex = hourOffset + relativeHour;
        minutesRemaining = (segmentDuration - std::fmod(elapsed, segmentDuration)) / 60.0;
    }

    double julianNoon = (static_cast<double>(targetMidnight) / 86400.0) + 2440588.0;
    int64_t JDN = static_cast<int64_t>(std::floor(julianNoon));
    int weekdayIndex = static_cast<int>((JDN + 1) % 7);

    Planet dayRuler = SUN;
    switch (weekdayIndex) {
        case 0: dayRuler = SUN;     break;
        case 1: dayRuler = MOON;    break;
        case 2: dayRuler = MARS;    break;
        case 3: dayRuler = MERCURY; break;
        case 4: dayRuler = JUPITER; break;
        case 5: dayRuler = VENUS;   break;
        case 6: dayRuler = SATURN;  break;
    }

    int planetIndex = (static_cast<int>(dayRuler) + hourIndex) % 7;
    Planet rulingPlanet = static_cast<Planet>(planetIndex);

    double segmentDurationFinal = isPolarActive ? 3600.0 : (totalDuration / 12.0);
    int relativeHourFinal = isPolarActive ? hourIndex : (hourIndex - hourOffset);

    int64_t hourStartUtc = activeStart + static_cast<int64_t>(relativeHourFinal * segmentDurationFinal);
    int64_t hourEndUtc   = activeStart + static_cast<int64_t>((relativeHourFinal + 1) * segmentDurationFinal);

    Dictionary info;
    info["ruling_planet"] = rulingPlanet;
    info["day_ruler"] = dayRuler;
    info["hour_index"] = hourIndex;
    info["minutes_remaining"] = minutesRemaining;
    info["planet_name"] = get_planet_name(rulingPlanet);
    info["hour_start_utc"] = hourStartUtc;
    info["hour_end_utc"] = hourEndUtc;

    return info;
}

String PlanetaryHourCalculator::get_planet_name(Planet planet) {
    switch (planet) {
        case SATURN:  return "Saturn";
        case JUPITER: return "Jupiter";
        case MARS:    return "Mars";
        case SUN:     return "Sun";
        case VENUS:   return "Venus";
        case MERCURY: return "Mercury";
        case MOON:    return "Moon";
    }
    return "Unknown";
}

} // namespace godot
