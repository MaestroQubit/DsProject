#pragma once
#include <string>
#include <iostream>
using namespace std;
class Graph {
public:
    struct EdgeNode { int destination; double travelTime; bool open; int roadId; EdgeNode* next; };
private:
    string* names; EdgeNode** heads; int vertexCount; int capacity; int nextRoadId;
    bool validVertex(int id) const {
        return id >= 0 && id < vertexCount;
    }
    EdgeNode* findEdge(int u,int v) const;
    bool insertOne(int u,int v,double time,bool open,int roadId);
    bool removeOne(int u,int v);
    void grow(); void freeEdges();
public:
    Graph(int initialCapacity=16); ~Graph();
    Graph(const Graph&) = delete; Graph& operator=(const Graph&) = delete;
    int addVertex(const string& name);
    bool addEdge(int u,int v,double travelTime,bool undirected=true);
    bool removeEdge(int u,int v);
    bool updateTraffic(int u,int v,double newTravelTime);
    bool closeRoad(int u,int v); bool reopenRoad(int u,int v);
    void displayGraph(ostream& out=cout) const;
    int getVertexCount() const {
        return vertexCount;
    }
    const string& getVertexName(int id) const;
    bool getEdgeWeight(int u,int v,double& weight) const;
    const EdgeNode* getAdjacencyHead(int vertex) const;
};
