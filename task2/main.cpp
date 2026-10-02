#include "cyan_detector.hpp"
#include "motion_math.hpp"

#include <opencv2/opencv.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

int main() {
    const std::string videoPath = "resources/task_2.mp4";
    const std::string csvPath = "result/task2/track.csv";

    // 培训文档已给出的固定旋转中心
    const cv::Point2f rotationCenter(480.0f, 360.0f);

    cv::VideoCapture video(videoPath);

    if (!video.isOpened()) {
        std::cerr << "Cannot open video: " << videoPath << '\n';
        return 1;
    }

    int width = static_cast<int>(
        video.get(cv::CAP_PROP_FRAME_WIDTH)
    );

    int height = static_cast<int>(
        video.get(cv::CAP_PROP_FRAME_HEIGHT)
    );

    double fps = video.get(cv::CAP_PROP_FPS);

    int totalFrames = static_cast<int>(
        video.get(cv::CAP_PROP_FRAME_COUNT)
    );

    if (fps <= 0.0) {
        std::cerr << "Invalid video FPS.\n";
        return 1;
    }

    double duration = totalFrames / fps;

    std::cout << "Width: " << width << '\n';
    std::cout << "Height: " << height << '\n';
    std::cout << "FPS: " << fps << '\n';
    std::cout << "Frames: " << totalFrames << '\n';
    std::cout << "Duration: " << duration << '\n';

    std::ofstream csv(csvPath);

    if (!csv.is_open()) {
        std::cerr << "Cannot create CSV: " << csvPath << '\n';
        return 1;
    }

    csv << "frame,time,x,y,wrapped_angle,unwrapped_angle\n";
    csv << std::fixed << std::setprecision(6);

    cv::Mat frame;

    int frameIndex = 0;
    int detectedCount = 0;

    bool hasPreviousAngle = false;
    double unwrappedAngle = 0.0;

    while (video.read(frame)) {
        cv::Point2f targetCenter;

        bool found = task2::detectCyanTarget(
            frame,
            targetCenter
        );

        double time =
            static_cast<double>(frameIndex) / fps;

        if (found) {
            double wrappedAngle = task2::calculateAngle(
                rotationCenter,
                targetCenter
            );

            if (!hasPreviousAngle) {
                // 第一帧没有上一帧可比较，直接使用原始角度
                unwrappedAngle = wrappedAngle;
                hasPreviousAngle = true;
            } else {
                unwrappedAngle = task2::unwrapAngle(
                    unwrappedAngle,
                    wrappedAngle
                );
            }

            csv << frameIndex << ','
                << time << ','
                << targetCenter.x << ','
                << targetCenter.y << ','
                << wrappedAngle << ','
                << unwrappedAngle << '\n';

            ++detectedCount;
        } else {
            csv << frameIndex << ','
                << time
                << ",nan,nan,nan,nan\n";
        }

        ++frameIndex;
    }

    csv.close();

    std::cout << "Processed frames: " << frameIndex << '\n';
    std::cout << "Detected targets: " << detectedCount << '\n';
    std::cout << "CSV saved to: " << csvPath << '\n';

    if (detectedCount != frameIndex) {
        std::cerr
            << "Tracking failed in "
            << frameIndex - detectedCount
            << " frames.\n";

        return 1;
    }

    return 0;
}