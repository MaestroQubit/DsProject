#include "../../include/routing/Graph.h"
#include <cmath>

using namespace std;

bool Graph::validVertex(int id) const
{
    return id >= 0 && id < vertexCount;
}

Graph::Graph(int initialCapacity)
{
    if (initialCapacity <= 0)
        initialCapacity = 1;

    vertexCount = 0;
    capacity = initialCapacity;
    nextRoadId = 1;
    names = new string[capacity];
    externalIds = new int[capacity];
    heads = new EdgeNode *[capacity];
    for (int i = 0; i < capacity; i++)
    {
        heads[i] = nullptr;
        externalIds[i] = -1;
    }
}

Graph::~Graph()
{
    freeEdges();
    delete[] names;
    delete[] externalIds;
    delete[] heads;
}

void Graph::freeEdges()
{
    for (int i = 0; i < vertexCount; i++)
    {
        while (heads[i] != nullptr)
        {
            EdgeNode *next =
                heads[i]->next;
            delete heads[i];
            heads[i] = next;
        }
    }
}

void Graph::clear()
{
    freeEdges();
    for (int i = 0; i < capacity; i++)
    {
        heads[i] = nullptr;
        externalIds[i] = -1;
        names[i].clear();
    }
    vertexCount = 0;
    nextRoadId = 1;
}

void Graph::grow()
{
    int newCapacity = capacity * 2;
    string *newNames = new string[newCapacity];
    int *newExternalIds = new int[newCapacity];
    EdgeNode **newHeads = new EdgeNode *[newCapacity];
    for (int i = 0; i < vertexCount; i++)
    {
        newNames[i] = names[i];
        newExternalIds[i] = externalIds[i];
        newHeads[i] = heads[i];
    }
    for (int i = vertexCount; i < newCapacity; i++)
    {
        newHeads[i] = nullptr;
        newExternalIds[i] = -1;
    }
    delete[] names;
    delete[] externalIds;
    delete[] heads;
    names = newNames;
    externalIds = newExternalIds;
    heads = newHeads;
    capacity = newCapacity;
}

int Graph::addVertex(const string &name, int externalId)
{
    if (vertexCount == capacity)
        grow();
    names[vertexCount] = name;
    externalIds[vertexCount] = (externalId == -1) ? vertexCount : externalId;
    heads[vertexCount] = nullptr;
    vertexCount++;
    return vertexCount - 1;
}

int Graph::getVertexIndex(int externalId) const
{
    for (int i = 0; i < vertexCount; i++)
    {
        if (externalIds[i] == externalId)
            return i;
    }
    if (externalId >= 0 && externalId < vertexCount && externalIds[externalId] == externalId)
        return externalId;
    return -1;
}

int Graph::getVertexExternalId(int vertexIndex) const
{
    if (!validVertex(vertexIndex))
        return -1;
    return externalIds[vertexIndex];
}

Graph::EdgeNode *Graph::findEdge(int u, int v) const
{
    if (!validVertex(u) || !validVertex(v))
        return nullptr;
    EdgeNode *edge = heads[u];
    while (edge)
    {
        if (edge->destination == v)
            return edge;
        edge = edge->next;
    }
    return nullptr;
}

bool Graph::insertOne(int u, int v, double time, bool open, int roadId)
{
    EdgeNode *old = findEdge(u, v);
    if (old)
    {
        old->travelTime = time;
        old->open = open;
        return false;
    }
    EdgeNode *edge = new EdgeNode;
    edge->destination = v;
    edge->travelTime = time;
    edge->open = open;
    edge->roadId = roadId;
    edge->next = heads[u];
    heads[u] = edge;
    return true;
}

bool Graph::addEdge(int u, int v, double time, bool undirected)
{
    int uIdx = getVertexIndex(u);
    int vIdx = getVertexIndex(v);
    if (!validVertex(uIdx) || !validVertex(vIdx) || time < 0 || !isfinite(time))
        return false;
    EdgeNode *existing = findEdge(uIdx, vIdx);
    int roadId = existing ? existing->roadId : nextRoadId++;
    insertOne(uIdx, vIdx, time, true, roadId);
    if (undirected)
        insertOne(vIdx, uIdx, time, true, roadId);
    return true;
}

bool Graph::removeOne(int u, int v)
{
    EdgeNode **current = &heads[u];
    while (*current)
    {
        if ((*current)->destination == v)
        {
            EdgeNode *temp = *current;
            *current = temp->next;
            delete temp;
            return true;
        }
        current = &((*current)->next);
    }
    return false;
}

bool Graph::removeEdge(int u, int v)
{
    int uIdx = getVertexIndex(u);
    int vIdx = getVertexIndex(v);
    if (!validVertex(uIdx) || !validVertex(vIdx))
        return false;
    EdgeNode *edge = findEdge(uIdx, vIdx);
    if (!edge)
        return false;
    int roadId = edge->roadId;
    removeOne(uIdx, vIdx);
    EdgeNode *reverse = findEdge(vIdx, uIdx);
    if (reverse && reverse->roadId == roadId)
        removeOne(vIdx, uIdx);
    return true;
}

bool Graph::updateTraffic(int u, int v, double time)
{
    if (time < 0 || !isfinite(time))
        return false;

    int uIdx = getVertexIndex(u);
    int vIdx = getVertexIndex(v);
    if (!validVertex(uIdx) || !validVertex(vIdx))
        return false;

    EdgeNode *edge = findEdge(uIdx, vIdx);
    if (!edge)
        return false;
    edge->travelTime = time;
    EdgeNode *reverse = findEdge(vIdx, uIdx);
    if (reverse && reverse->roadId == edge->roadId)
        reverse->travelTime = time;
    return true;
}

bool Graph::closeRoad(int u, int v)
{
    int uIdx = getVertexIndex(u);
    int vIdx = getVertexIndex(v);
    if (!validVertex(uIdx) || !validVertex(vIdx))
        return false;

    EdgeNode *edge = findEdge(uIdx, vIdx);
    if (!edge)
        return false;
    edge->open = false;
    EdgeNode *reverse = findEdge(vIdx, uIdx);
    if (reverse && reverse->roadId == edge->roadId)
        reverse->open = false;
    return true;
}

bool Graph::reopenRoad(int u, int v)
{
    int uIdx = getVertexIndex(u);
    int vIdx = getVertexIndex(v);
    if (!validVertex(uIdx) || !validVertex(vIdx))
        return false;

    EdgeNode *edge = findEdge(uIdx, vIdx);
    if (!edge)
        return false;
    edge->open = true;
    EdgeNode *reverse = findEdge(vIdx, uIdx);
    if (reverse && reverse->roadId == edge->roadId)
        reverse->open = true;
    return true;
}

void Graph::displayGraph(ostream &out) const
{
    for (int i = 0; i < vertexCount; i++)
        out << externalIds[i] << " " << names[i] << '\n';
}

const string &Graph::getVertexName(int id) const
{
    static string invalid = "<invalid>";
    int idx = getVertexIndex(id);
    if (!validVertex(idx))
        return invalid;
    return names[idx];
}

bool Graph::getEdgeWeight(int u, int v, double &weight) const
{
    int uIdx = getVertexIndex(u);
    int vIdx = getVertexIndex(v);
    if (!validVertex(uIdx) || !validVertex(vIdx))
        return false;
    EdgeNode *edge = findEdge(uIdx, vIdx);
    if (!edge)
        return false;
    weight = edge->travelTime;
    return true;
}

const Graph::EdgeNode *Graph::getAdjacencyHead(int vertex) const
{
    if (validVertex(vertex))
        return heads[vertex];
    int vIdx = getVertexIndex(vertex);
    if (validVertex(vIdx))
        return heads[vIdx];
    return nullptr;
}