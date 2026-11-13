#include "../../include/routing/RouteManager.h"
#include <vector>
#include <limits>
#include <iomanip>
#include <algorithm>

using namespace std;

RouteManager::RouteManager(const Graph &cityGraph) : graph(cityGraph) {}

RouteResult RouteManager::shortestPath(int source, int destination) const
{
    RouteResult result;

    result.source = source;
    result.destination = destination;

    int n = graph.getVertexCount();
    int srcIdx = graph.getVertexIndex(source);
    int destIdx = graph.getVertexIndex(destination);

    if (srcIdx < 0 || destIdx < 0 || srcIdx >= n || destIdx >= n)
        return result;

    vector<double> distance(n, numeric_limits<double>::infinity());
    vector<int> parent(n, -1);
    vector<bool> visited(n, false);

    MinHeap heap;
    distance[srcIdx] = 0.0;
    heap.insert(srcIdx, 0.0);

    HeapNode current;
    while (heap.extractMin(current))
    {
        int u = current.vertex;
        if (visited[u] || current.distance != distance[u])
            continue;
        visited[u] = true;
        if (u == destIdx)
            break;
        const Graph::EdgeNode *edge = graph.getAdjacencyHead(u);
        while (edge)
        {
            int v = edge->destination;
            double nd = distance[u] + edge->travelTime;
            if (edge->open && nd < distance[v])
            {
                distance[v] = nd;
                parent[v] = u;
                heap.insert(v, nd);
            }
            edge = edge->next;
        }
    }

    if (!visited[destIdx])
        return result;

    result.routeExists = true;
    result.totalTravelTime = distance[destIdx];
    vector<int> path;
    for (int v = destIdx; v != -1; v = parent[v])
        path.push_back(graph.getVertexExternalId(v));
    reverse(path.begin(), path.end());
    result.path = path;
    return result;
}

double RouteManager::getDistance(int source, int destination) const
{
    return shortestPath(source, destination).totalTravelTime;
}

double RouteManager::calculateTravelTime(int source, int destination) const
{
    return getDistance(source, destination);
}

void RouteManager::printPath(const RouteResult &result, ostream &out) const
{
    if (!result.routeExists)
    {
        out << "NO ROUTE AVAILABLE\n";
        return;
    }
    for (size_t i = 0; i < result.path.size(); i++)
    {
        out << graph.getVertexName(result.path[i]);
        if (i + 1 < result.path.size())
            out << " -> ";
    }
    out << "\nTotal Travel Time: " << fixed << setprecision(2) << result.totalTravelTime << " minutes\n";
}