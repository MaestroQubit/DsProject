#pragma once
#include "Graph.h"
#include <string>

using namespace std;

struct LocationInfo {
    int locationId;
    string name;
    string type;
};

void buildCityMap(Graph& graph, const string& restaurantFile = "restaurants.txt");
const LocationInfo* getLocationCatalog(int& count);
