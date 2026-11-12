#pragma once
#include "Graph.h"
#include "MinHeap.h"
#include "RouteResult.h"
#include <iomanip>
#include <limits>
using namespace std;

class RouteManager {
private:
    const Graph& graph;

public:
    RouteManager(const Graph& cityGraph) : graph(cityGraph) {
    }

    RouteResult shortestPath(int source, int destination) const {
        RouteResult result;
        result.source = source;
        result.destination = destination;

        int n = graph.getVertexCount();
        if (source < 0 || source >= n || destination < 0 || destination >= n) {
            return result;
        }

        double* distance = new double[n];
        int* parent = new int[n];
        bool* visited = new bool[n];

        for (int i = 0; i < n; i++) {
            distance[i] = numeric_limits<double>::infinity();
            parent[i] = -1;
            visited[i] = false;
        }

        MinHeap heap;
        distance[source] = 0.0;
        heap.insert(source, 0.0);

        HeapNode current;
        while (heap.extractMin(current)) {
            int u = current.vertex;
            if (u < 0 || u >= n || visited[u] || current.distance != distance[u]) {
                continue;
            }

            visited[u] = true;
            if (u == destination) {
                break;
            }

            const Graph::EdgeNode* edge = graph.getAdjacencyHead(u);
            while (edge != nullptr) {
                int v = edge->destination;
                if (edge->open && !visited[v] &&
                    distance[u] <= numeric_limits<double>::max() - edge->travelTime) {
                    double newDistance = distance[u] + edge->travelTime;
                    if (newDistance < distance[v]) {
                        distance[v] = newDistance;
                        parent[v] = u;
                        heap.insert(v, newDistance);
                    }
                }
                edge = edge->next;
            }
        }

        if (visited[destination]) {
            int count = 0;
            for (int vertex = destination; vertex != -1; vertex = parent[vertex]) {
                count++;
            }

            result.path = new int[count];
            result.pathLength = count;
            int vertex = destination;
            for (int i = count - 1; i >= 0; i--) {
                result.path[i] = vertex;
                vertex = parent[vertex];
            }
            result.routeExists = true;
            result.totalTravelTime = distance[destination];
        }

        delete[] distance;
        delete[] parent;
        delete[] visited;
        return result;
    }

    double getDistance(int source, int destination) const {
        RouteResult result = shortestPath(source, destination);
        return result.totalTravelTime;
    }

    double calculateTravelTime(int source, int destination) const {
        return getDistance(source, destination);
    }

    void printPath(const RouteResult& result, ostream& out) const {
        if (!result.routeExists) {
            out << "NO ROUTE AVAILABLE.\n";
            return;
        }

        out << "Source: " << graph.getVertexName(result.source) << "\n";
        out << "Destination: " << graph.getVertexName(result.destination) << "\n";
        for (int i = 0; i < result.pathLength; i++) {
            out << graph.getVertexName(result.path[i]);
            if (i + 1 < result.pathLength) {
                double time;
                graph.getEdgeWeight(result.path[i], result.path[i + 1], time);
                out << "\n     | (" << time << " min)\n";
            }
        }
        out << "\nTotal Travel Time: " << fixed << setprecision(2)
            << result.totalTravelTime << " minutes\n";
    }
};
