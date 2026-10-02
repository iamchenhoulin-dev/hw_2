#include "cyan_detector.hpp"
#include "motion_math.hpp"

#include <opencv2/opencv.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>

int main()
{
    cv::VideoCapture video(
        "resources/task_2.mp4"
    );

    if (!video.isOpened())
    {
        std::cerr
            << "Cannot open video\n";

        return 1;
    }


    int width =
        static_cast<int>(
            video.get(cv::CAP_PROP_FRAME_WIDTH)
        );

    int height =
        static_cast<int>(
            video.get(cv::CAP_PROP_FRAME_HEIGHT)
        );

    double fps =
        video.get(cv::CAP_PROP_FPS);


    cv::VideoWriter writer;

    writer.open(
        "result/task2/annotated.mp4",
        cv::VideoWriter::fourcc(
            'm','p','4','v'
        ),
        fps,
        cv::Size(width,height)
    );


    if (!writer.isOpened())
    {
        std::cerr
            << "Cannot create video\n";

        return 1;
    }


    cv::Point2f center(
        480.0f,
        360.0f
    );

    double omega;
    double theta0;
    double rmse;


    std::ifstream paramFile(
        "result/task2/parameters.txt"
    );


    if (!paramFile.is_open())
    {
        std::cerr
            << "Cannot open parameters.txt\n";

        return 1;
    }


    std::string line;


    std::getline(
        paramFile,
        line
    );

    // Angular velocity(rad/s): 1.34775
    omega = std::stod(
        line.substr(
            line.find(":") + 1
        )
    );


    std::getline(
        paramFile,
        line
    );

    // Initial angle(rad): 0.632815
    theta0 = std::stod(
        line.substr(
            line.find(":") + 1
        )
    );


    std::getline(
        paramFile,
        line
    );


    // SSE
    std::getline(
        paramFile,
        line
    );

    // RMSE(rad): 0.232363
    rmse = std::stod(
        line.substr(
            line.find(":") + 1
        )
    );


    paramFile.close();

    cv::Mat frame;

    int frameIndex = 0;


    while(video.read(frame))
    {
        cv::Point2f target;


        if(task2::detectCyanTarget(
            frame,
            target))
        {

            cv::circle(
                frame,
                target,
                8,
                cv::Scalar(
                    0,255,0
                ),
                2
            );


            cv::line(
                frame,
                center,
                target,
                cv::Scalar(
                    255,0,0
                ),
                2
            );


            double angle =
                task2::calculateAngle(
                    center,
                    target
                );


            std::stringstream text;

            text
                << std::fixed
                << std::setprecision(3)
                << "theta="
                << angle
                << " rad";


            cv::putText(
                frame,
                text.str(),
                cv::Point(30,50),
                cv::FONT_HERSHEY_SIMPLEX,
                1,
                cv::Scalar(
                    0,255,255
                ),
                2
            );
        }


        std::stringstream info;

        info
            << "omega="
            << omega
            << " rad/s";


        cv::putText(
            frame,
            info.str(),
            cv::Point(30,90),
            cv::FONT_HERSHEY_SIMPLEX,
            1,
            cv::Scalar(
                0,255,255
            ),
            2
        );


        writer.write(frame);

        frameIndex++;
    }


    writer.release();


    std::cout
        << "Saved annotated video\n";


    return 0;
}