#pragma once

#include <opencv2/core.hpp>

namespace task2 {

bool detectCyanTarget(
    const cv::Mat& frame,
    cv::Point2f& center,
    cv::Mat* mask = nullptr
);

}