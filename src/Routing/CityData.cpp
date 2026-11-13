#include "../../include/routing/CityData.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <filesystem>

using namespace std;

static vector<LocationInfo> locationData;

static string resolvePath(const string &path)
{
    if (path.empty())
        return path;
    if (filesystem::exists(path))
        return path;
    string p1 = "../" + path;
    if (filesystem::exists(p1))
        return p1;
    string p2 = "../../" + path;
    if (filesystem::exists(p2))
        return p2;
    string p3 = "../../../" + path;
    if (filesystem::exists(p3))
        return p3;
    return path;
}

static string resolveDirectory(const string &dir)
{
    if (!dir.empty() && filesystem::exists(dir))
        return dir;
    const char *candidates[] = {"data", "../data", "../../data", "../../../data"};
    for (const char *cand : candidates)
    {
        if (filesystem::exists(cand) && filesystem::is_directory(cand))
        {
            if (filesystem::exists(string(cand) + "/locations.csv"))
                return cand;
        }
    }
    return dir.empty() ? "data" : dir;
}

const LocationInfo *getLocationCatalog(int &count)
{
    count = static_cast<int>(locationData.size());
    if (locationData.empty())
        return nullptr;
    return locationData.data();
}

bool loadLocations(const string &filename, vector<LocationInfo> &locations)
{
    locations.clear();
    string resolved = resolvePath(filename);
    ifstream locationFile(resolved);
    if (!locationFile)
        return false;

    string line;
    if (!getline(locationFile, line))
        return false;

    while (getline(locationFile, line))
    {
        if (line.empty())
            continue;
        if (line.back() == '\r')
            line.pop_back();
        if (line.empty())
            continue;

        stringstream ss(line);
        string token;
        LocationInfo info;

        if (!getline(ss, token, ',') || !getline(ss, info.name, ',') ||
            !getline(ss, info.type))
            return false;

        if (!info.type.empty() && info.type.back() == '\r')
            info.type.pop_back();

        try
        {
            info.locationId = stoi(token);
        }
        catch (...)
        {
            return false;
        }

        if (info.locationId < 0)
            return false;

        locations.push_back(info);
    }
    return !locations.empty();
}

bool loadRoads(const string &filename, Graph &graph)
{
    string resolved = resolvePath(filename);
    ifstream roadFile(resolved);
    if (!roadFile)
        return false;

    string line;
    if (!getline(roadFile, line))
        return false;

    while (getline(roadFile, line))
    {
        if (line.empty())
            continue;
        if (line.back() == '\r')
            line.pop_back();
        if (line.empty())
            continue;

        stringstream ss(line);
        string token;

        int source = -1;
        int destination = -1;
        double time = 0.0;

        if (!getline(ss, token, ','))
            return false;
        try
        {
            source = stoi(token);
            if (!getline(ss, token, ','))
                return false;
            destination = stoi(token);
            if (!getline(ss, token))
                return false;
            if (!token.empty() && token.back() == '\r')
                token.pop_back();
            time = stod(token);
        }
        catch (...)
        {
            return false;
        }

        if (source < 0 || destination < 0 || time < 0 || !isfinite(time))
            return false;

        if (!graph.addEdge(source, destination, time))
            return false;
    }
    return true;
}

bool buildCityMap(Graph &graph, const string &dataDir)
{
    locationData.clear();
    string dir = resolveDirectory(dataDir);
    string locPath = dir + "/locations.csv";
    string roadPath = dir + "/roads.csv";

    if (!loadLocations(locPath, locationData))
        return false;

    for (const LocationInfo &info : locationData)
    {
        graph.addVertex(info.name, info.locationId);
    }

    if (!loadRoads(roadPath, graph))
        return false;

    return true;
}
