//
// Created by 13920 on 2023/8/08.
//

#ifndef FORTUNE_HORIZONTALCHECKER_H
#define FORTUNE_HORIZONTALCHECKER_H


#include <vector>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>

class HorizontalChecker {
private:
    static const int maxNum = 300;
public:
    typedef std::pair<double, double> Point;

    static bool check(const std::vector<Point>& points) {

        if (points.size() > maxNum) {
            throw std::length_error("Too many points to check.");
        }

        // No duplicate x-coordinates when there's only one or zero points
        if (points.size() <= 1) {
            return true;
        }

        // Use an unordered set to keep track of encountered x-coordinates
        std::unordered_set<double> xCoordinates;

        for (const auto& point : points) {
            if (!xCoordinates.insert(point.first).second) {
                return false; // Found a duplicate x-coordinate
            }
        }
        return true; // All points have distinct x-coordinates
    }
};






#endif //FORTUNE_HORIZONTALCHECKER_H
