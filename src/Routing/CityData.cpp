#include "../../include/routing/CityData.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
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

void buildCityMap(Graph &graph)
{
    locationData.clear();
    ifstream locationFile("data/locations.csv");
    if (!locationFile)
        return;

    string line;

    if (!getline(locationFile, line))
        return;

    while (getline(locationFile, line))
    {
        stringstream ss(line);
        string token;
        LocationInfo info;

        if (!getline(ss, token, ',') || !getline(ss, info.name, ',') ||
            !getline(ss, info.type))
            return;

        try
        {
            info.locationId = stoi(token);
        }
        catch (const invalid_argument &)
        {
            return;
        }
        catch (const out_of_range &)
        {
            return;
        }

        locationData.push_back(info);

        graph.addVertex(info.name);
    }
    locationFile.close();
    ifstream roadFile("data/roads.csv");
    if (!roadFile || !getline(roadFile, line))
        return;

    while (getline(roadFile, line))
    {
        stringstream ss(line);
        string token;

        int source = -1;
        int destination = -1;
        double time = 0.0;

        if (!getline(ss, token, ','))
            return;
        try
        {
            source = stoi(token);
            if (!getline(ss, token, ','))
                return;
            destination = stoi(token);
            if (!getline(ss, token))
                return;
            time = stod(token);
        }
        catch (const invalid_argument &)
        {
            return;
        }
        catch (const out_of_range &)
        {
            return;
        }

        graph.addEdge(source, destination, time);
    }
    roadFile.close();
}