#pragma once

#include <string>
#include <iostream>

class Graph
{
public:
    struct EdgeNode
    {
        int destination;
        double travelTime;
        bool open;
        int roadId;
        EdgeNode *next;
    };

private:
    std::string *names;
    int *externalIds;
    EdgeNode **heads;
    int vertexCount;
    int capacity;
    int nextRoadId;
    bool validVertex(int id) const;

    EdgeNode *findEdge(int u, int v) const;
    bool insertOne(int u, int v, double time, bool open, int roadId);
    bool removeOne(int u, int v);
    void grow();
    void freeEdges();

public:
    Graph(int initialCapacity = 16);
    ~Graph();
    Graph(const Graph &) = delete;
    Graph &operator=(const Graph &) = delete;
    const EdgeNode *getAdjacencyHead(int vertex) const;
    const std::string &getVertexName(int id) const;

    int addVertex(const std::string &name, int externalId = -1);
    int getVertexIndex(int externalId) const;
    int getVertexExternalId(int vertexIndex) const;
    bool addEdge(int u, int v, double travelTime, bool undirected = true);
    bool removeEdge(int u, int v);
    bool updateTraffic(int u, int v, double newTravelTime);
    bool closeRoad(int u, int v);
    bool reopenRoad(int u, int v);
    void displayGraph(std::ostream &out = std::cout) const;
    void clear();
    int getVertexCount() const { return vertexCount; }
    bool getEdgeWeight(int u, int v, double &weight) const;
};