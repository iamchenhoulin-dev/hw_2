#include "cyan_detector.hpp"

#include <opencv2/imgproc.hpp>
#include <cmath>
#include <vector>

namespace task2 {

bool detectCyanTarget(
    const cv::Mat& frame,
    cv::Point2f& center,
    cv::Mat* mask
) {
    if (frame.empty()) {
        return false;
    }

    // 1. BGR 转换为 HSV
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    // 2. 提取青色区域
    cv::Mat cyanMask;
    cv::inRange(
        hsv,
        cv::Scalar(80, 100, 100),
        cv::Scalar(100, 255, 255),
        cyanMask
    );

    // 3. 开运算：去掉零散噪点
    cv::Mat kernel = cv::getStructuringElement(
        cv::MORPH_ELLIPSE,
        cv::Size(3, 3)
    );

    cv::morphologyEx(
        cyanMask,
        cyanMask,
        cv::MORPH_OPEN,
        kernel
    );

    // 如果调用者需要二值图，就复制出去
    if (mask != nullptr) {
        cyanMask.copyTo(*mask);
    }

    // 4. 查找青色区域的外轮廓
    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(
        cyanMask,
        contours,
        cv::RETR_EXTERNAL,
        cv::CHAIN_APPROX_SIMPLE
    );

    // 5. 寻找面积最大的轮廓
    int bestIndex = -1;
    double largestArea = 0.0;

    for (int i = 0; i < static_cast<int>(contours.size()); ++i) {
        double area = cv::contourArea(contours[i]);

        if (area > largestArea) {
            largestArea = area;
            bestIndex = i;
        }
    }

    // 面积太小，认为只是噪声
    if (bestIndex == -1 || largestArea < 20.0) {
        return false;
    }

    // 6. 用图像矩计算轮廓重心
    cv::Moments moments = cv::moments(contours[bestIndex]);

    if (std::abs(moments.m00) < 1e-6) {
        return false;
    }

    center.x = static_cast<float>(moments.m10 / moments.m00);
    center.y = static_cast<float>(moments.m01 / moments.m00);

    return true;
}

}