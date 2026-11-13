#pragma once

#include "Graph.h"

#include <vector>
#include <string>

struct LocationInfo
{
    int locationId;
    std::string name;
    std::string type;
};

bool loadLocations(const std::string &filename, std::vector<LocationInfo> &locations);
bool loadRoads(const std::string &filename, Graph &graph);
bool buildCityMap(Graph &graph, const std::string &dataDir = "data");
const LocationInfo *getLocationCatalog(int &count);
