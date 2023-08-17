//
// Created by ShuoRen on 2023-08-08.
//

#ifndef FORTUNE_POINTGENERATOR_H
#define FORTUNE_POINTGENERATOR_H



#include <random>
#include <ctime>
#include <chrono>



class PointGenerator {

private:
    unsigned seed;
    std::mt19937 gen;
    int numPoints;
public:
    typedef std::pair<double, double> point;

    PointGenerator() {
        // 使用当前时间作为种子
        seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        gen = std::mt19937(seed);
    }

    std::vector<point> generatePoints(int num, double minX, double maxX, double minY, double maxY) {
        this->numPoints = num;
        std::uniform_real_distribution<> x_dist(minX, maxX);
        std::uniform_real_distribution<> y_dist(minY, maxY);

        std::vector<point> points;

        for (int i = 0; i < numPoints; ++i) {
            double x = x_dist(gen);
            double y = y_dist(gen);
            points.emplace_back(x, y);
        }

        return points;
    }

};









#endif //FORTUNE_POINTGENERATOR_H
