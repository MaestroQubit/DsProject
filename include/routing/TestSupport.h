#pragma once
#include "Graph.h"
#include "RouteManager.h"
#include <iostream>
#include <cmath>
using namespace std;
extern int testsRun, testsFailed;
inline void expect(bool condition, const char *name)
{
    ++testsRun;
    if (!condition)
    {
        ++testsFailed;
        cerr << "FAIL: " << name << '\n';
    }
}
inline bool near(double a, double b) { return fabs(a - b) < 1e-9; }
void runMinHeapTests();
void runGraphTests();
void runDijkstraTests();
