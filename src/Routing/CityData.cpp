#include "../../include/routing/CityData.h"
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

static vector<LocationInfo> locationData;
const LocationInfo *getLocationCatalog(int &count)
{
    count = static_cast<int>(locationData.size());
    if (locationData.empty())
        return nullptr;
    return locationData.data();
}

void buildCityMap(Graph &graph, const string &restaurantFile)
{
    locationData.clear();
    ifstream locationFile("data/locations.csv");
    string line;

    getline(locationFile, line);

    while (getline(locationFile, line))
    {
        stringstream ss(line);
        string token;
        LocationInfo info;

        getline(ss, token, ',');
        info.locationId = stoi(token);
        getline(ss, info.name, ',');
        getline(ss, info.type, ',');
        locationData.push_back(info);

        graph.addVertex(info.name);
    }

    locationFile.close();
    ifstream roadFile("data/roads.csv");
    getline(roadFile, line);

    while (getline(roadFile, line))
    {
        stringstream ss(line);
        string token;

        int source;
        int destination;
        double time;

        getline(ss, token, ',');
        source = stoi(token);
        getline(ss, token, ',');
        destination = stoi(token);
        getline(ss, token, ',');
        time = stod(token);

        graph.addEdge(source, destination, time);
    }
    roadFile.close();
}