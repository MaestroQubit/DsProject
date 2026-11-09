#include "../../include/routing/Graph.h"
#include <cmath>
using namespace std;

Graph::Graph(int initialCapacity)
{
    if (initialCapacity <= 0)
        initialCapacity = 1;
    vertexCount = 0;
    capacity = initialCapacity;
    nextRoadId = 1;
    names = new string[capacity];
    heads = new EdgeNode *[capacity];
    for (int i = 0; i < capacity; i++)
        heads[i] = nullptr;
}
Graph::~Graph()
{
    freeEdges();
    delete[] names;
    delete[] heads;
}
void Graph::freeEdges()
{
    for (int i = 0; i < vertexCount; i++)
    {
        while (heads[i] != nullptr)
        {
            EdgeNode *next = heads[i]->next;
            delete heads[i];
            heads[i] = next;
        }
    }
}
void Graph::grow()
{
    int newCapacity = capacity * 2;
    string *newNames = new string[newCapacity];
    EdgeNode **newHeads = new EdgeNode *[newCapacity];
    for (int i = 0; i < vertexCount; i++)
    {
        newNames[i] = names[i];
        newHeads[i] = heads[i];
    }
    for (int i = vertexCount; i < newCapacity; i++)
        newHeads[i] = nullptr;
    delete[] names;
    delete[] heads;
    names = newNames;
    heads = newHeads;
    capacity = newCapacity;
}
int Graph::addVertex(const string &name)
{
    if (vertexCount == capacity)
        grow();
    names[vertexCount] = name;
    heads[vertexCount] = nullptr;
    vertexCount++;
    return vertexCount - 1;
}
Graph::EdgeNode *Graph::findEdge(int u, int v) const
{
    if (!validVertex(u) || !validVertex(v))
        return nullptr;
    EdgeNode *edge = heads[u];
    while (edge != nullptr)
    {
        if (edge->destination == v)
            return edge;
        edge = edge->next;
    }
    return nullptr;
}
bool Graph::insertOne(int u, int v, double time, bool open, int roadId)
{
    EdgeNode *oldEdge = findEdge(u, v);
    if (oldEdge != nullptr)
    {
        oldEdge->travelTime = time;
        oldEdge->open = open;
        oldEdge->roadId = roadId;
        return false;
    }
    EdgeNode *edge = new EdgeNode{v, time, open, roadId, heads[u]};
    heads[u] = edge;
    return true;
}
bool Graph::addEdge(int u, int v, double time, bool undirected)
{
    if (!validVertex(u) || !validVertex(v) || time < 0 || !std::isfinite(time))
        return false;
    EdgeNode *forward = findEdge(u, v);
    EdgeNode *reverse = undirected ? findEdge(v, u) : nullptr;
    int roadId;
    if (forward != nullptr)
        roadId = forward->roadId;
    else if (reverse != nullptr)
        roadId = reverse->roadId;
    else
        roadId = nextRoadId++;
    insertOne(u, v, time, true, roadId);
    if (undirected)
        insertOne(v, u, time, true, roadId);
    return true;
}
bool Graph::removeOne(int u, int v)
{
    EdgeNode **current = &heads[u];
    while (*current != nullptr)
    {
        if ((*current)->destination == v)
        {
            EdgeNode *removed = *current;
            *current = removed->next;
            delete removed;
            return true;
        }
        current = &((*current)->next);
    }
    return false;
}
bool Graph::removeEdge(int u, int v)
{
    EdgeNode *edge = findEdge(u, v);
    if (edge == nullptr)
        return false;
    int roadId = edge->roadId;
    removeOne(u, v);
    EdgeNode *reverse = findEdge(v, u);
    if (reverse != nullptr && reverse->roadId == roadId)
        removeOne(v, u);
    return true;
}
bool Graph::updateTraffic(int u, int v, double time)
{
    if (time < 0 || !isfinite(time))
        return false;
    EdgeNode *edge = findEdge(u, v);
    if (edge == nullptr)
        return false;
    edge->travelTime = time;
    EdgeNode *reverse = findEdge(v, u);
    if (reverse != nullptr && reverse->roadId == edge->roadId)
        reverse->travelTime = time;
    return true;
}
bool Graph::closeRoad(int u, int v)
{
    EdgeNode *edge = findEdge(u, v);
    if (edge == nullptr)
        return false;
    edge->open = false;
    EdgeNode *reverse = findEdge(v, u);
    if (reverse != nullptr && reverse->roadId == edge->roadId)
        reverse->open = false;
    return true;
}
bool Graph::reopenRoad(int u, int v)
{
    EdgeNode *edge = findEdge(u, v);
    if (edge == nullptr)
        return false;
    edge->open = true;
    EdgeNode *reverse = findEdge(v, u);
    if (reverse != nullptr && reverse->roadId == edge->roadId)
        reverse->open = true;
    return true;
}
void Graph::displayGraph(ostream &out) const
{
    for (int i = 0; i < vertexCount; i++)
    {
        out << i << ". " << names[i] << ":";
        EdgeNode *edge = heads[i];
        while (edge != nullptr)
        {
            out << " -> [" << edge->destination << " " << names[edge->destination] << ", " << edge->travelTime << " min, " << (edge->open ? "open" : "closed") << "]";
            edge = edge->next;
        }
        out << '\n';
    }
}
const string &Graph::getVertexName(int id) const
{
    static const string invalidName = "<invalid location>";
    if (!validVertex(id))
        return invalidName;
    return names[id];
}
bool Graph::getEdgeWeight(int u, int v, double &weight) const
{
    EdgeNode *edge = findEdge(u, v);
    if (edge == nullptr)
        return false;
    weight = edge->travelTime;
    return true;
}
const Graph::EdgeNode *Graph::getAdjacencyHead(int vertex) const
{
    if (!validVertex(vertex))
        return nullptr;
    return heads[vertex];
}
