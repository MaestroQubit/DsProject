#include "../../include/routing/TestSupport.h"
void runGraphTests()
{
    Graph g(1);
    int a = g.addVertex("A"), b = g.addVertex("B"), c = g.addVertex("C");
    expect(g.getVertexCount() == 3, "vertex growth");
    expect(g.addEdge(a, b, 2), "add undirected edge");
    double w = 0;
    expect(g.getEdgeWeight(a, b, w) && near(w, 2), "edge weight");
    expect(g.getEdgeWeight(b, a, w), "reverse undirected edge");
    expect(g.updateTraffic(a, b, 4.5) && g.getEdgeWeight(b, a, w) && near(w, 4.5), "traffic both directions");
    expect(g.closeRoad(a, b), "close road");
    expect(!g.getAdjacencyHead(a)->open && !g.getAdjacencyHead(b)->open, "closure both directions");
    expect(g.reopenRoad(a, b) && g.getAdjacencyHead(a)->open, "reopen road");
    expect(g.removeEdge(a, b) && !g.getEdgeWeight(a, b, w) && !g.getEdgeWeight(b, a, w), "remove undirected edge");
    expect(g.addEdge(a, c, 1, false) && g.getEdgeWeight(a, c, w) && !g.getEdgeWeight(c, a, w), "directed behavior");
    expect(!g.addEdge(-1, b, 1) && !g.addEdge(a, b, -1) && !g.updateTraffic(a, b, 2), "invalid graph operations");
    Graph mixed;
    mixed.addVertex("A");
    mixed.addVertex("B");
    expect(mixed.addEdge(0, 1, 5.0, false), "add initial directed road");
    expect(mixed.addEdge(1, 0, 7.0, true), "complete directed road as undirected");
    expect(mixed.closeRoad(1, 0), "close paired road");
    expect(!mixed.getAdjacencyHead(0)->open && !mixed.getAdjacencyHead(1)->open, "paired roads share status");
    expect(mixed.reopenRoad(0, 1), "reopen paired road");
    expect(mixed.getAdjacencyHead(0)->open && mixed.getAdjacencyHead(1)->open, "paired roads reopen together");
}