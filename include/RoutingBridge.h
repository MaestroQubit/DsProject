#pragma once
#include "RouteManager.h"
#include <string>

using namespace std;

struct RouteDisplayData {
    bool routeFound;
    int sourceId;
    int destinationId;
    double totalMinutes;
    string sourceName;
    string destinationName;
    string routeText;
    int* locationIds;
    int locationCount;

    RouteDisplayData() {
        routeFound = false;
        sourceId = -1;
        destinationId = -1;
        totalMinutes = 0.0;
        locationIds = nullptr;
        locationCount = 0;
    }

    ~RouteDisplayData() {
        delete[] locationIds;
    }

    RouteDisplayData(const RouteDisplayData&) = delete;
    RouteDisplayData& operator=(const RouteDisplayData&) = delete;

    RouteDisplayData(RouteDisplayData&& other) noexcept {
        routeFound = other.routeFound;
        sourceId = other.sourceId;
        destinationId = other.destinationId;
        totalMinutes = other.totalMinutes;
        sourceName = other.sourceName;
        destinationName = other.destinationName;
        routeText = other.routeText;
        locationIds = other.locationIds;
        locationCount = other.locationCount;
        other.locationIds = nullptr;
        other.locationCount = 0;
    }

    RouteDisplayData& operator=(RouteDisplayData&& other) noexcept {
        if (this != &other) {
            delete[] locationIds;
            routeFound = other.routeFound;
            sourceId = other.sourceId;
            destinationId = other.destinationId;
            totalMinutes = other.totalMinutes;
            sourceName = other.sourceName;
            destinationName = other.destinationName;
            routeText = other.routeText;
            locationIds = other.locationIds;
            locationCount = other.locationCount;
            other.locationIds = nullptr;
            other.locationCount = 0;
        }
        return *this;
    }
};

class RoutingBridge {
private:
    const Graph& graph;
    RouteManager routeManager;

public:
    RoutingBridge(const Graph& cityGraph) : graph(cityGraph), routeManager(cityGraph) {
    }

    RouteDisplayData findRoute(int sourceId, int destinationId) const;
    double getEstimatedMinutes(int sourceId, int destinationId) const;
};
