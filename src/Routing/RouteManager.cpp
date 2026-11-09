#include "../../include/routing/RouteManager.h"
#include <iomanip>
#include <limits>
#include <vector>
using namespace std;

RouteManager::RouteManager(const Graph &cityGraph) : graph(cityGraph) {}

RouteResult RouteManager::shortestPath(int source, int destination) const
{
    RouteResult result;
    result.source = source;
    result.destination = destination;

    int n = graph.getVertexCount();
    if (source < 0 || source >= n || destination < 0 || destination >= n)
    {
        return result;
    }

    double *distance = new double[n];
    int *parent = new int[n];
    bool *visited = new bool[n];

    for (int i = 0; i < n; i++)
    {
        distance[i] = numeric_limits<double>::infinity();
        parent[i] = -1;
        visited[i] = false;
    }

    MinHeap heap;
    distance[source] = 0.0;
    heap.insert(source, 0.0);

    HeapNode current;
    while (heap.extractMin(current))
    {
        int u = current.vertex;
        if (u < 0 || u >= n || visited[u] || current.distance != distance[u])
        {
            continue;
        }

        visited[u] = true;
        if (u == destination)
        {
            break;
        }

        const Graph::EdgeNode *edge = graph.getAdjacencyHead(u);
        while (edge != nullptr)
        {
            int v = edge->destination;
            if (edge->open && !visited[v] &&
                distance[u] <= numeric_limits<double>::max() - edge->travelTime)
            {
                double newDistance = distance[u] + edge->travelTime;
                if (newDistance < distance[v])
                {
                    distance[v] = newDistance;
                    parent[v] = u;
                    heap.insert(v, newDistance);
                }
            }
            edge = edge->next;
        }
    }

    if (visited[destination])
    {
        vector<int> reversedPath;
        for (int vertex = destination; vertex != -1; vertex = parent[vertex])
            reversedPath.push_back(vertex);
        result.path.assign(reversedPath.rbegin(), reversedPath.rend());
        result.routeExists = true;
        result.totalTravelTime = distance[destination];
    }

    delete[] distance;
    delete[] parent;
    delete[] visited;
    return result;
}

double RouteManager::getDistance(int source, int destination) const
{
    RouteResult result = shortestPath(source, destination);
    return result.totalTravelTime;
}

double RouteManager::calculateTravelTime(int source, int destination) const
{
    return getDistance(source, destination);
}

void RouteManager::printPath(const RouteResult &result, ostream &out) const
{
    if (!result.routeExists)
    {
        out << "NO ROUTE AVAILABLE.\n";
        return;
    }

    out << "Source: " << graph.getVertexName(result.source) << "\n";
    out << "Destination: " << graph.getVertexName(result.destination) << "\n";
    for (int i = 0; i < result.path.size(); i++)
    {
        out << graph.getVertexName(result.path[i]);
        if (i + 1 < result.path.size())
        {
            double time;
            graph.getEdgeWeight(result.path[i], result.path[i + 1], time);
            out << "\n     | (" << time << " min)\n";
        }
    }
    out << "\nTotal Travel Time: " << fixed << setprecision(2)
        << result.totalTravelTime << " minutes\n";
}