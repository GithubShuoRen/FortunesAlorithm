//
// Created by ShuoRen on 2023-08-07.
//

#ifndef FORTUNE_ROTATOR_H
#define FORTUNE_ROTATOR_H


#include <chrono>
#include <random>
#include <vector>

using namespace std;

class Rotator {
public:
    typedef std::pair<double, double> Point;

    Rotator() {
        // 使用当前时间作为种子
        seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        gen = std::mt19937(seed);
        thetaDis = std::uniform_real_distribution<>(0, 2 * M_PI);
        theta = thetaDis(gen);  // 初始化随机角度
    }


    std::vector<Point> rotatePoints(std::vector<Point>& inputPoints) {
        theta = thetaDis(gen);  // 更新随机角度
        vector<Point> newPoints;
        for (auto& p : inputPoints) {
            p = rotatePoint(p, theta);
            Point newP = p;
            newPoints.push_back(newP);
        }
        return newPoints;
    }

    std::vector<Point> reverseRotatePoints(const std::vector<Point>& inputPoints) {
        double reverse_theta = -theta;
        std::vector<Point> reversedPoints;

        for (const auto& p : inputPoints) {
            reversedPoints.push_back(rotatePoint(p, reverse_theta));
        }

        return reversedPoints;
    }

private:
    unsigned seed;
    std::mt19937 gen;
    std::uniform_real_distribution<> thetaDis;
    double theta;

    Point rotatePoint(const Point& p, double angle) {
        double cosA = std::cos(angle);
        double sinA = std::sin(angle);
        double x = cosA * p.first - sinA * p.second;
        double y = sinA * p.first + cosA * p.second;
        return std::make_pair(x, y);
    }
};





#endif //FORTUNE_ROTATOR_H
