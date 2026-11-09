#pragma once
#include "Graph.h"
#include "MinHeap.h"
#include "RouteResult.h"
#include <iomanip>
#include <limits>
using namespace std;

class RouteManager
{
private:
    const Graph &graph;

public:
    RouteManager(const Graph &cityGraph);

    RouteResult shortestPath(int source, int destination) const;
    double getDistance(int source, int destination) const;
    double calculateTravelTime(int source, int destination) const;
    void printPath(const RouteResult& result, ostream& out) const;
};
