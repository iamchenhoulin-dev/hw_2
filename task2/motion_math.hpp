#pragma once

#include <opencv2/core.hpp>

namespace task2 {

double calculateAngle(
    const cv::Point2f& origin,
    const cv::Point2f& point
);

double unwrapAngle(
    double previousUnwrappedAngle,
    double currentWrappedAngle
);
}