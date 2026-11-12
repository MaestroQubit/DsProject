#pragma once

struct HeapNode
{
    int vertex;
    double distance;
};

class MinHeap
{
private:
    HeapNode *data;
    int size;
    int capacity;
    void grow();
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    MinHeap(int initialCapacity = 16);
    ~MinHeap();
    MinHeap(const MinHeap &) = delete;
    MinHeap &operator=(const MinHeap &) = delete;
    void insert(int vertex, double distance);
    bool extractMin(HeapNode &result);
    bool isEmpty() const;
    int getSize() const;
    void clear();
};