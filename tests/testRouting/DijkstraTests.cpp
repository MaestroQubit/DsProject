#include "../../include/routing/TestSupport.h"
#include <limits>
using namespace std;
int testsRun = 0, testsFailed = 0;
static double bellmanFord(const Graph &g, int s, int d)
{
    int n = g.getVertexCount();
    if (s < 0 || d < 0 || s >= n || d >= n)
        return numeric_limits<double>::infinity();
    double *x = new double[n];
    for (int i = 0; i < n; ++i)
        x[i] = numeric_limits<double>::infinity();
    x[s] = 0;
    for (int k = 1; k < n; ++k)
    {
        bool changed = false;
        for (int u = 0; u < n; ++u)
            if (x[u] != numeric_limits<double>::infinity())
                for (const Graph::EdgeNode *e = g.getAdjacencyHead(u); e; e = e->next)
                    if (e->open && x[u] + e->travelTime < x[e->destination])
                    {
                        x[e->destination] = x[u] + e->travelTime;
                        changed = true;
                    }
        if (!changed)
            break;
    }
    double ans = x[d];
    delete[] x;
    return ans;
}
static void validate(const Graph &g, const RouteResult &r)
{
    if (!r.routeExists)
        return;
    expect(!r.path.empty() && r.path.front() == r.source && r.path.back() == r.destination, "path endpoints");
    double sum = 0;
    for (size_t i = 0; i + 1 < r.path.size(); ++i)
    {
        double w;
        expect(g.getEdgeWeight(r.path[i], r.path[i + 1], w), "path uses valid edge");
        sum += w;
    }
    expect(near(sum, r.totalTravelTime), "path total matches distance");
}
void runDijkstraTests()
{
    Graph g;
    for (int i = 0; i < 7; ++i)
    {
        g.addVertex("L" + to_string(i));
    }
    g.addEdge(0, 1, 5);
    g.addEdge(0, 2, 1);
    g.addEdge(2, 1, 1);
    g.addEdge(1, 3, 2);
    g.addEdge(2, 3, 10);
    g.addEdge(3, 4, 0);
    g.addEdge(4, 5, 1.5);
    RouteManager rm(g);
    RouteResult r = rm.shortestPath(0, 5);
    expect(r.routeExists && near(r.totalTravelTime, 5.5), "relaxation finds improved route");
    validate(g, r);
    expect(!rm.shortestPath(0, 6).routeExists, "disconnected unreachable");
    RouteResult self = rm.shortestPath(2, 2);
    expect(self.routeExists && self.path.size() == 1 && near(self.totalTravelTime, 0), "source equals destination");
    expect(!rm.shortestPath(-1, 2).routeExists && !rm.shortestPath(0, 99).routeExists, "invalid endpoints");
    g.closeRoad(1, 3);
    RouteResult altered = rm.shortestPath(0, 5);
    expect(altered.routeExists && near(altered.totalTravelTime, 12.5), "closure changes route");
    g.updateTraffic(2, 3, 1);
    RouteResult traffic = rm.shortestPath(0, 5);
    expect(near(traffic.totalTravelTime, 3.5), "traffic update changes route");
    g.updateTraffic(2, 3, 10);
    g.reopenRoad(1, 3);
    expect(near(rm.getDistance(0, 5), 5.5), "reopening restores original route");
    Graph directed;
    for (int i = 0; i < 3; ++i)
    {
        directed.addVertex("D");
    }
    directed.addEdge(0, 1, 1, false);
    directed.addEdge(1, 2, 2, false);
    RouteManager dr(directed);
    expect(near(dr.getDistance(0, 2), 3) && !dr.shortestPath(2, 0).routeExists, "directed routing");
    Graph random;
    for (int i = 0; i < 20; ++i)
        random.addVertex("N" + to_string(i));
    for (int i = 0; i < 20; ++i)
        for (int j = i + 1; j < 20; ++j)
        {
            if (((i * 31 + j * 17) % 5) == 0)
                random.addEdge(i, j, ((i * 7 + j * 3) % 11) / 2.0, ((i + j) % 2) == 0);
        }
    RouteManager oracleTarget(random);
    for (int s = 0; s < 20; ++s)
        for (int d = 0; d < 20; ++d)
        {
            double expected = bellmanFord(random, s, d), actual = oracleTarget.getDistance(s, d);
            expect((isinf(expected) && isinf(actual)) || near(expected, actual), "Bellman-Ford distance oracle");
            RouteResult q = oracleTarget.shortestPath(s, d);
            if (q.routeExists)
                validate(random, q);
        }
}
int main()
{
    runMinHeapTests();
    runGraphTests();
    runDijkstraTests();
    cout << "Tests run: " << testsRun << ", failures: " << testsFailed << '\n';
    return testsFailed ? 1 : 0;
}
