#include "../include/RoutingBridge.h"
#include <sstream>
#include <iomanip>

using namespace std;

RouteDisplayData RoutingBridge::findRoute(int sourceId, int destinationId) const {
    RouteDisplayData display;
    RouteResult result = routeManager.shortestPath(sourceId, destinationId);

    display.sourceId = sourceId;
    display.destinationId = destinationId;
    display.sourceName = graph.getVertexName(sourceId);
    display.destinationName = graph.getVertexName(destinationId);
    display.routeFound = result.routeExists;

    if (!result.routeExists) {
        display.routeText = "NO ROUTE AVAILABLE.";
        return display;
    }

    display.totalMinutes = result.totalTravelTime;
    display.locationCount = result.pathLength;
    display.locationIds = new int[display.locationCount];
    stringstream text;
    text << display.sourceName << " to " << display.destinationName << "\n";

    for (int i = 0; i < result.pathLength; i++) {
        display.locationIds[i] = result.path[i];
        text << graph.getVertexName(result.path[i]);
        if (i + 1 < result.pathLength) {
            double time;
            graph.getEdgeWeight(result.path[i], result.path[i + 1], time);
            text << " -> " << fixed << setprecision(2) << time << " min -> ";
        }
    }
    text << "\nTotal Travel Time: " << fixed << setprecision(2) << display.totalMinutes << " minutes";
    display.routeText = text.str();
    return display;
}

double RoutingBridge::getEstimatedMinutes(int sourceId, int destinationId) const {
    return routeManager.getDistance(sourceId, destinationId);
}
