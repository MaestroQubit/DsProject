#ifndef KITCHEN_HEAP_H
#define KITCHEN_HEAP_H

#include <vector>

struct KitchenOrder {
    int orderId;
    int restaurantId;
    int prepTime;
    int priority;
    int score;

    KitchenOrder();
    KitchenOrder(int orderId, int restaurantId, int prepTime, int priority);
};

class KitchenHeap {
private:
    std::vector<KitchenOrder> data;

    bool isBefore(const KitchenOrder& a, const KitchenOrder& b) const;
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    void insert(const KitchenOrder& order);
    bool extractMin(KitchenOrder& result);
    bool peekMin(KitchenOrder& result) const;
    bool removeOrder(int orderId, KitchenOrder& removed);
    bool isEmpty() const;
    void clear();
    int size() const;
};

#endif
