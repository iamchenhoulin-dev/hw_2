#include <opencv2/opencv.hpp>
#include <iostream>
//#include<vector>
using namespace cv;
int main() {
    Mat img = imread("resources/test_image.jpg");
    if (img.empty()) {
        std::cerr << "Cannot read image!\n";
        return 1;
    }
    imshow("Original", img);
    /*

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    imshow("Gray", gray);


    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    std::vector<Mat> hsvChannels;
    split(hsv, hsvChannels);

    imwrite("result/task1/gray.png", gray);
    imwrite("result/task1/hsv_h.png", hsvChannels[0]);
    imwrite("result/task1/hsv_s.png", hsvChannels[1]);
    imwrite("result/task1/hsv_v.png", hsvChannels[2]);

    imshow("Gray", gray);
    imshow("H", hsvChannels[0]);
    imshow("S", hsvChannels[1]);
    imshow("V", hsvChannels[2]);


    Mat meanImg, gaussianImg, medianImg;
    blur(img, meanImg, Size(5, 5));
    GaussianBlur(img, gaussianImg, Size(5, 5), 1.5);
    medianBlur(img, medianImg, 5);

    imwrite("result/task1/mean_filter.png", meanImg);
    imwrite("result/task1/gaussian_filter.png", gaussianImg);
    imwrite("result/task1/median_filter.png", medianImg);

    imshow("Mean", meanImg);
    imshow("Gaussian", gaussianImg);
    imshow("Median", medianImg);
    

    Mat hsv, maskLow, maskHigh, mask;
    cvtColor(img, hsv, COLOR_BGR2HSV);
    // Red spans both ends of the hue interval.
    inRange(hsv, Scalar(0, 100, 100),
    Scalar(10, 255, 255), maskLow);
    inRange(hsv, Scalar(170, 100, 100),
    Scalar(179, 255, 255), maskHigh);
    bitwise_or(maskLow, maskHigh, mask);
    imwrite("result/task1/red_mask.png", mask);
    imshow("Red mask", mask);


    Mat gray, binary;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    threshold(gray, binary, 128, 255, THRESH_BINARY);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat dilated, eroded, opened, closed;

    dilate(binary, dilated, kernel);
    erode(binary, eroded, kernel);
    morphologyEx(binary, opened, MORPH_OPEN, kernel);
    morphologyEx(binary, closed, MORPH_CLOSE, kernel);
    
    imwrite("result/task1/erode.png", eroded);
    imwrite("result/task1/dilate.png", dilated);
    imwrite("result/task1/open.png", opened);
    imwrite("result/task1/close.png", closed);

    imshow("Erode", eroded);
    imshow("Dilate", dilated);
    imshow("Open", opened);
    imshow("Close", closed);
    */

    Mat gray, edges, morph;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    Canny(gray, edges, 100, 200);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(edges, morph, MORPH_CLOSE, kernel);std::vector<std::vector<Point>> contours;std::vector<Vec4i> hierarchy;
    findContours(morph, contours, hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    Mat contourImg = Mat::zeros(img.size(), CV_8UC3);
    drawContours(contourImg, contours, -1, Scalar(0, 255, 0), 2);
    //imwrite("result/task1/contours.png",contourImg);
    imshow("Contours", contourImg);


    Mat result = img.clone();
    for (size_t i = 0; i < contours.size(); ++i) {
        double area = contourArea(contours[i]);
        if (area < 500.0) continue;
        Rect box = boundingRect(contours[i]);
        double ratio = static_cast<double>(box.width)/box.height;
        if (ratio < 0.2 || ratio > 5.0) continue;
        rectangle(result, box, Scalar(0, 0, 255), 2);
        drawContours(result, contours, static_cast<int>(i),Scalar(0, 255, 0), 2);
    }
    imwrite("result/task1/contours_boxes.png",result);
    imshow("Filtered contours", result);


    Mat drawing = img.clone();
    Point imageCenter(img.cols / 2, img.rows / 2);
    circle(
        drawing,
        imageCenter,
        80,
        Scalar(0, 255, 0),
        4
    );
    rectangle(
        drawing,
        Point(img.cols / 10, img.rows / 10),
        Point(img.cols / 3, img.rows / 3),
        Scalar(255, 0, 0),
        4
    );
    putText(
        drawing,
        "Hello I am chl",
        Point(50, 50),
        FONT_HERSHEY_SIMPLEX,
        1.2,
        Scalar(0, 0, 255),
        3
    );
    imwrite("result/task1/drawing.png", drawing);
    imshow("Drawing", drawing);


    Point2f rotationCenter(
        img.cols / 2.0F,
        img.rows / 2.0F
    );

    Mat rotationMatrix = getRotationMatrix2D(
        rotationCenter,
        35.0,
        1.0
    );

    Mat rotated;

    warpAffine(
        img,
        rotated,
        rotationMatrix,
        img.size(),
        INTER_LINEAR,
        BORDER_CONSTANT,
        Scalar(0, 0, 0)
    );

    imwrite("result/task1/rotated_35deg.png", rotated);
    imshow("Rotated 35 Degrees", rotated);


    Rect cropRegion(
        0,
        0,
        img.cols / 2,
        img.rows / 2
    );

    Mat cropped = img(cropRegion).clone();

    imwrite(
        "result/task1/crop_top_left.png",
        cropped
    );

    imshow("Top Left Crop", cropped);
    waitKey(0);
    return 0;
}