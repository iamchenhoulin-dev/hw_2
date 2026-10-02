#include "motion_math.hpp"

#include <cmath>

namespace task2 {

double calculateAngle(
    const cv::Point2f& origin,
    const cv::Point2f& point
) {
    double dx = point.x - origin.x;
    double dy = origin.y - point.y;

    return std::atan2(dy, dx);
}

double unwrapAngle(
    double previousUnwrappedAngle,
    double currentWrappedAngle
) {
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;

    double difference =
        currentWrappedAngle - previousUnwrappedAngle;

    while (difference > PI) {
        difference -= TWO_PI;
    }

    while (difference < -PI) {
        difference += TWO_PI;
    }

    return previousUnwrappedAngle + difference;
}

}