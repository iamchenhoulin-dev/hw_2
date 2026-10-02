#include <Eigen/Dense>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


int main()
{
    std::ifstream file(
        "result/task2/track.csv"
    );

    if (!file.is_open())
    {
        std::cerr
            << "Cannot open track.csv\n";

        return 1;
    }


    std::vector<double> times;
    std::vector<double> angles;


    std::string line;

    // 跳过表头
    std::getline(file, line);


    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        std::string item;


        // frame
        std::getline(ss, item, ',');

        // time
        std::getline(ss, item, ',');
        double t = std::stod(item);


        // x
        std::getline(ss, item, ',');

        // y
        std::getline(ss, item, ',');

        // wrapped_angle
        std::getline(ss, item, ',');


        // unwrapped_angle
        std::getline(ss, item, ',');
        double theta = std::stod(item);


        times.push_back(t);
        angles.push_back(theta);
    }


    int n = times.size();


    Eigen::MatrixXd A(n, 2);

    Eigen::VectorXd y(n);


    for (int i = 0; i < n; i++)
    {
        A(i,0) = times[i];
        A(i,1) = 1.0;

        y(i) = angles[i];
    }


    Eigen::Vector2d result =
        A.colPivHouseholderQr()
         .solve(y);


    double omega = result[0];
    double theta0 = result[1];

    std::ofstream fitFile(
        "result/task2/fit.csv"
    );

    fitFile
        << "time,real_angle,fit_angle,error\n";


    for (int i = 0; i < n; i++)
    {
        double fitted =
            omega * times[i] + theta0;

        double error =
            angles[i] - fitted;


        fitFile
            << times[i] << ","
            << angles[i] << ","
            << fitted << ","
            << error
            << "\n";
    }


    fitFile.close();


    Eigen::VectorXd residual =
        A * result - y;


    double SSE =
        residual.squaredNorm();

    double RMSE =
    std::sqrt(
        SSE / n
    );

    std::cout
        << "RMSE = "
        << RMSE
        << "\n";

    std::cout
        << "omega = "
        << omega
        << "\n";


    std::cout
        << "theta0 = "
        << theta0
        << "\n";


    std::cout
        << "SSE = "
        << SSE
        << "\n";

    std::ofstream resultFile(
        "result/task2/parameters.txt"
    );

    resultFile
        << "Angular velocity(rad/s): "
        << omega
        << "\n";

    resultFile
        << "Initial angle(rad): "
        << theta0
        << "\n";

    resultFile
        << "SSE: "
        << SSE
        << "\n";

    resultFile
        << "RMSE(rad): "
        << RMSE
        << "\n";

    resultFile.close();

    return 0;
}