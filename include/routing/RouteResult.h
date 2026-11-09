#pragma once
#include <limits>
#include <vector>
struct RouteResult
{
    bool routeExists = false;
    int source = -1;
    int destination = -1;
    double totalTravelTime = std::numeric_limits<double>::infinity();
    std::vector<int> path;
};