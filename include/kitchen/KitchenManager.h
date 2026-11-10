#ifndef KITCHEN_MANAGER_H
#define KITCHEN_MANAGER_H

#include "KitchenHeap.h"

struct QueueNode {
    KitchenOrder order;
    QueueNode* next;
};

class OrderQueue {
private:
    QueueNode* front;
    QueueNode* rear;
    int count;
    int load;

public:
    OrderQueue();
    ~OrderQueue();
    OrderQueue(const OrderQueue&) = delete;
    OrderQueue& operator=(const OrderQueue&) = delete;

    void enqueue(const KitchenOrder& order);
    bool dequeue(KitchenOrder& result);
    bool isEmpty() const;
    int size() const;
    int totalLoad() const;
    void print() const;
};

class KitchenManager {
private:
    static const int STATIONS = 3;
    KitchenHeap heap;
    OrderQueue stations[STATIONS];

    int leastLoadedStation() const;

public:
    void addOrder(int orderId, int restaurantId, int prepTime, int priority);
    void sendToStations();
    bool completeOrder(int station, KitchenOrder& readyOrder);
    int pendingOrders() const;
    int stationQueueSize(int station) const;
    int stationCount() const;
    void showStatus() const;
};

#endif
