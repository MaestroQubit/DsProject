#include "TestSupport.h"
#include <limits>

using namespace std;

int testsRun = 0;
int testsFailed = 0;

static double bellmanFord(const Graph& graph, int source, int destination) {
    int n = graph.getVertexCount();
    if (source < 0 || destination < 0 || source >= n || destination >= n) {
        return numeric_limits<double>::infinity();
    }

    double* distance = new double[n];
    for (int i = 0; i < n; i++) {
        distance[i] = numeric_limits<double>::infinity();
    }
    distance[source] = 0.0;

    for (int step = 1; step < n; step++) {
        bool changed = false;
        for (int u = 0; u < n; u++) {
            if (distance[u] == numeric_limits<double>::infinity()) {
                continue;
            }
            const Graph::EdgeNode* edge = graph.getAdjacencyHead(u);
            while (edge != nullptr) {
                if (edge->open && distance[u] + edge->travelTime < distance[edge->destination]) {
                    distance[edge->destination] = distance[u] + edge->travelTime;
                    changed = true;
                }
                edge = edge->next;
            }
        }
        if (!changed) {
            break;
        }
    }

    double answer = distance[destination];
    delete[] distance;
    return answer;
}

static void validatePath(const Graph& graph, const RouteResult& result) {
    if (!result.routeExists) {
        return;
    }

    expect(result.pathLength > 0, "path has vertices");
    expect(result.path[0] == result.source, "path starts at source");
    expect(result.path[result.pathLength - 1] == result.destination, "path ends at destination");

    double total = 0.0;
    for (int i = 0; i + 1 < result.pathLength; i++) {
        double edgeWeight;
        expect(graph.getEdgeWeight(result.path[i], result.path[i + 1], edgeWeight), "path has valid edge");
        total += edgeWeight;
    }
    expect(near(total, result.totalTravelTime), "path weight equals reported distance");
}

void runDijkstraTests() {
    Graph graph;
    for (int i = 0; i < 7; i++) {
        graph.addVertex("L" + to_string(i));
    }

    graph.addEdge(0, 1, 5);
    graph.addEdge(0, 2, 1);
    graph.addEdge(2, 1, 1);
    graph.addEdge(1, 3, 2);
    graph.addEdge(2, 3, 10);
    graph.addEdge(3, 4, 0);
    graph.addEdge(4, 5, 1.5);

    RouteManager routes(graph);
    RouteResult route = routes.shortestPath(0, 5);
    expect(route.routeExists && near(route.totalTravelTime, 5.5), "relaxation finds improved route");
    validatePath(graph, route);

    expect(!routes.shortestPath(0, 6).routeExists, "disconnected destination");
    RouteResult sameLocation = routes.shortestPath(2, 2);
    expect(sameLocation.routeExists && sameLocation.pathLength == 1 && near(sameLocation.totalTravelTime, 0), "source equals destination");
    expect(!routes.shortestPath(-1, 2).routeExists, "invalid source");
    expect(!routes.shortestPath(0, 99).routeExists, "invalid destination");

    graph.closeRoad(1, 3);
    expect(near(routes.getDistance(0, 5), 12.5), "road closure changes route");
    graph.updateTraffic(2, 3, 1);
    expect(near(routes.getDistance(0, 5), 3.5), "traffic update changes route");
    graph.updateTraffic(2, 3, 10);
    graph.reopenRoad(1, 3);
    expect(near(routes.getDistance(0, 5), 5.5), "road reopening restores route");

    Graph directed;
    for (int i = 0; i < 3; i++) {
        directed.addVertex("D");
    }
    directed.addEdge(0, 1, 1, false);
    directed.addEdge(1, 2, 2, false);
    RouteManager directedRoutes(directed);
    expect(near(directedRoutes.getDistance(0, 2), 3), "directed route works");
    expect(!directedRoutes.shortestPath(2, 0).routeExists, "reverse directed route unavailable");

    Graph generatedGraph;
    for (int i = 0; i < 20; i++) {
        generatedGraph.addVertex("N" + to_string(i));
    }
    for (int i = 0; i < 20; i++) {
        for (int j = i + 1; j < 20; j++) {
            if ((i * 31 + j * 17) % 5 == 0) {
                generatedGraph.addEdge(i, j, ((i * 7 + j * 3) % 11) / 2.0, (i + j) % 2 == 0);
            }
        }
    }

    RouteManager generatedRoutes(generatedGraph);
    for (int source = 0; source < 20; source++) {
        for (int destination = 0; destination < 20; destination++) {
            double expected = bellmanFord(generatedGraph, source, destination);
            double actual = generatedRoutes.getDistance(source, destination);
            bool bothUnreachable = isinf(expected) && isinf(actual);
            expect(bothUnreachable || near(expected, actual), "Bellman-Ford distance oracle");
            RouteResult generatedRoute = generatedRoutes.shortestPath(source, destination);
            validatePath(generatedGraph, generatedRoute);
        }
    }
}

int main() {
    runMinHeapTests();
    runGraphTests();
    runDijkstraTests();
    cout << "Tests run: " << testsRun << ", failures: " << testsFailed << '\n';
    return testsFailed == 0 ? 0 : 1;
}
