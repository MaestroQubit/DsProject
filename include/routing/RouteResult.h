#pragma once
#include <limits>
using namespace std;

struct RouteResult
{
    bool routeExists;
    int source;
    int destination;
    double totalTravelTime;
    int *path;
    int pathLength;

    RouteResult() : routeExists(false), source(-1), destination(-1),
                    totalTravelTime(numeric_limits<double>::infinity()), path(nullptr), pathLength(0) {}

    ~RouteResult()
    {
        delete[] path;
    }

    RouteResult(const RouteResult &other) : routeExists(other.routeExists), source(other.source), destination(other.destination), totalTravelTime(other.totalTravelTime), path(nullptr), pathLength(other.pathLength)
    {
        if (pathLength > 0)
        {
            path = new int[pathLength];
            for (int i = 0; i < pathLength; i++)
            {
                path[i] = other.path[i];
            }
        }
    }

    RouteResult &operator=(const RouteResult &other)
    {
        if (this == &other)
        {
            return *this;
        }
        int *newPath = nullptr;
        if (other.pathLength > 0)
        {
            newPath = new int[other.pathLength];
            for (int i = 0; i < other.pathLength; i++)
            {
                newPath[i] = other.path[i];
            }
        }
        delete[] path;
        path = newPath;
        routeExists = other.routeExists;
        source = other.source;
        destination = other.destination;
        totalTravelTime = other.totalTravelTime;
        pathLength = other.pathLength;
        return *this;
    }

    RouteResult(RouteResult &&other) noexcept : routeExists(other.routeExists), source(other.source), destination(other.destination), totalTravelTime(other.totalTravelTime), path(other.path), pathLength(other.pathLength)
    {
        other.path = nullptr;
        other.pathLength = 0;
    }

    RouteResult &operator=(RouteResult &&other) noexcept
    {
        if (this != &other)
        {
            delete[] path;
            routeExists = other.routeExists;
            source = other.source;
            destination = other.destination;
            totalTravelTime = other.totalTravelTime;
            path = other.path;
            pathLength = other.pathLength;
            other.path = nullptr;
            other.pathLength = 0;
        }
        return *this;
    }
};
