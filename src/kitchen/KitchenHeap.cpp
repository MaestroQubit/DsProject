#include "../../include/kitchen/KitchenHeap.h"

KitchenOrder::KitchenOrder() {
    orderId = 0;
    restaurantId = 0;
    prepTime = 0;
    priority = 0;
    score = 0;
}

KitchenOrder::KitchenOrder(int orderId, int restaurantId, int prepTime, int priority) {
    this->orderId = orderId;
    this->restaurantId = restaurantId;
    this->prepTime = prepTime;
    this->priority = priority;
    this->score = prepTime - priority * 5;
}

bool KitchenHeap::isBefore(const KitchenOrder& a, const KitchenOrder& b) const {
    if (a.score != b.score) {
        return a.score < b.score;
    }
    return a.orderId < b.orderId;
}

void KitchenHeap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (isBefore(data[index], data[parent])) {
            KitchenOrder temp = data[index];
            data[index] = data[parent];
            data[parent] = temp;
            index = parent;
        } else {
            break;
        }
    }
}

void KitchenHeap::heapifyDown(int index) {
    int n = data.size();
    while (true) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < n && isBefore(data[left], data[smallest])) {
            smallest = left;
        }
        if (right < n && isBefore(data[right], data[smallest])) {
            smallest = right;
        }
        if (smallest == index) {
            break;
        }

        KitchenOrder temp = data[index];
        data[index] = data[smallest];
        data[smallest] = temp;
        index = smallest;
    }
}

void KitchenHeap::insert(const KitchenOrder& order) {
    data.push_back(order);
    heapifyUp(data.size() - 1);
}

bool KitchenHeap::extractMin(KitchenOrder& result) {
    if (data.empty()) {
        return false;
    }
    result = data[0];
    data[0] = data.back();
    data.pop_back();
    if (!data.empty()) {
        heapifyDown(0);
    }
    return true;
}

bool KitchenHeap::peekMin(KitchenOrder& result) const {
    if (data.empty()) {
        return false;
    }
    result = data[0];
    return true;
}

bool KitchenHeap::removeOrder(int orderId, KitchenOrder& removed) {
    for (size_t i = 0; i < data.size(); i++) {
        if (data[i].orderId == orderId) {
            removed = data[i];
            if (i == data.size() - 1) {
                data.pop_back();
                return true;
            }
            data[i] = data.back();
            data.pop_back();
            heapifyDown(i);
            heapifyUp(i);
            return true;
        }
    }
    return false;
}

bool KitchenHeap::isEmpty() const {
    return data.empty();
}

void KitchenHeap::clear() {
    data.clear();
}

int KitchenHeap::size() const {
    return data.size();
}
