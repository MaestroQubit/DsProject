#include "../include/MinHeap.h"
#include <utility>
using namespace std;

MinHeap::MinHeap(int initialCapacity) {
    if (initialCapacity <= 0) initialCapacity = 1;
    capacity = initialCapacity;
    size = 0;
    data = new HeapNode[capacity];
}
MinHeap::~MinHeap() { delete[] data; }
void MinHeap::grow() {
    int newCapacity = capacity * 2;
    HeapNode* newData = new HeapNode[newCapacity];
    for (int i = 0; i < size; i++) newData[i] = data[i];
    delete[] data;
    data = newData;
    capacity = newCapacity;
}
void MinHeap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        bool parentIsSmaller = data[parent].distance < data[index].distance ||
            (data[parent].distance == data[index].distance && data[parent].vertex <= data[index].vertex);
        if (parentIsSmaller) break;
        swap(data[parent], data[index]);
        index = parent;
    }
}
void MinHeap::heapifyDown(int index) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;
        if (left < size && (data[left].distance < data[smallest].distance ||
            (data[left].distance == data[smallest].distance && data[left].vertex < data[smallest].vertex))) smallest = left;
        if (right < size && (data[right].distance < data[smallest].distance ||
            (data[right].distance == data[smallest].distance && data[right].vertex < data[smallest].vertex))) smallest = right;
        if (smallest == index) break;
        swap(data[index], data[smallest]);
        index = smallest;
    }
}
void MinHeap::insert(int vertex, double distance) {
    if (size == capacity) grow();
    data[size].vertex = vertex;
    data[size].distance = distance;
    heapifyUp(size);
    size++;
}
bool MinHeap::extractMin(HeapNode& result) {
    if (size == 0) return false;
    result = data[0];
    size--;
    if (size > 0) {
        data[0] = data[size];
        heapifyDown(0);
    }
    return true;
}
