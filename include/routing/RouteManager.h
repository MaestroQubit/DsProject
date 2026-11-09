#pragma once
#include "Graph.h"
#include "MinHeap.h"
#include "RouteResult.h"
#include <iosfwd>
class RouteManager
{
private:
    const Graph &graph;

public:
    explicit RouteManager(const Graph &cityGraph);

    RouteResult shortestPath(int source, int destination) const;
    double getDistance(int source, int destination) const;
    double calculateTravelTime(int source, int destination) const;
    void printPath(const RouteResult& result, std::ostream& out) const;
};
