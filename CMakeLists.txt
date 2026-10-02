cmake_minimum_required(VERSION 3.16)

project(hw_2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(Ceres REQUIRED)

add_executable(task1 task1/main.cpp)

target_include_directories(task1 PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(task1 PRIVATE
    ${OpenCV_LIBS}
)

add_executable(task2
    task2/main.cpp
    task2/cyan_detector.cpp
    task2/motion_math.cpp
)

target_include_directories(task2 PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/task2
)

target_link_libraries(task2 PRIVATE
    ${OpenCV_LIBS}
)

add_executable(task2_fit
    task2/linear_fit.cpp
)

target_link_libraries(task2_fit PRIVATE
    Eigen3::Eigen
)

add_executable(task2_annotate
    task2/annotate_video.cpp
    task2/cyan_detector.cpp
    task2/motion_math.cpp
)


target_include_directories(task2_annotate PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/task2
)


target_link_libraries(task2_annotate PRIVATE
    ${OpenCV_LIBS}
)