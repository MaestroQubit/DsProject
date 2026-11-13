#include "../../include/kitchen/KitchenManager.h"
#include <iostream>

OrderQueue::OrderQueue() {
    front = nullptr;
    rear = nullptr;
    count = 0;
    load = 0;
}

OrderQueue::~OrderQueue() {
    clear();
}

void OrderQueue::clear() {
    while (front != nullptr) {
        QueueNode* temp = front;
        front = front->next;
        delete temp;
    }
    rear = nullptr;
    count = 0;
    load = 0;
}

void OrderQueue::enqueue(const KitchenOrder& order) {
    QueueNode* node = new QueueNode;
    node->order = order;
    node->next = nullptr;

    if (rear == nullptr) {
        front = node;
        rear = node;
    } else {
        rear->next = node;
        rear = node;
    }
    count++;
    load += order.prepTime;
}

bool OrderQueue::dequeue(KitchenOrder& result) {
    if (front == nullptr) {
        return false;
    }
    QueueNode* temp = front;
    result = temp->order;
    front = front->next;
    if (front == nullptr) {
        rear = nullptr;
    }
    delete temp;
    count--;
    load -= result.prepTime;
    return true;
}

bool OrderQueue::peek(KitchenOrder& result) const {
    if (front == nullptr) {
        return false;
    }
    result = front->order;
    return true;
}

bool OrderQueue::removeOrder(int orderId, KitchenOrder& removed) {
    QueueNode* current = front;
    QueueNode* prev = nullptr;
    while (current != nullptr) {
        if (current->order.orderId == orderId) {
            removed = current->order;
            if (prev == nullptr) {
                front = current->next;
                if (front == nullptr) {
                    rear = nullptr;
                }
            } else {
                prev->next = current->next;
                if (current == rear) {
                    rear = prev;
                }
            }
            delete current;
            count--;
            load -= removed.prepTime;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

bool OrderQueue::isEmpty() const {
    return front == nullptr;
}

int OrderQueue::size() const {
    return count;
}

int OrderQueue::totalLoad() const {
    return load;
}

void OrderQueue::print() const {
    QueueNode* current = front;
    if (current == nullptr) {
        std::cout << "empty";
    }
    while (current != nullptr) {
        std::cout << "[Order " << current->order.orderId << ", prep " << current->order.prepTime << "] ";
        current = current->next;
    }
}

int KitchenManager::leastLoadedStation() const {
    int best = 0;
    for (int i = 1; i < STATIONS; i++) {
        if (stations[i].totalLoad() < stations[best].totalLoad()) {
            best = i;
        }
    }
    return best;
}

void KitchenManager::addOrder(int orderId, int restaurantId, int prepTime, int priority) {
    KitchenOrder order(orderId, restaurantId, prepTime, priority);
    heap.insert(order);
}

void KitchenManager::sendToStations() {
    KitchenOrder order;
    while (heap.extractMin(order)) {
        int station = leastLoadedStation();
        stations[station].enqueue(order);
        std::cout << "Order " << order.orderId << " sent to Cook " << station + 1 << "\n";
    }
}

bool KitchenManager::completeOrder(int station, KitchenOrder& readyOrder) {
    if (station < 0 || station >= STATIONS) {
        return false;
    }
    return stations[station].dequeue(readyOrder);
}

bool KitchenManager::peekStationOrder(int station, KitchenOrder& order) const {
    if (station < 0 || station >= STATIONS) {
        return false;
    }
    return stations[station].peek(order);
}

bool KitchenManager::removeOrder(int orderId, KitchenOrder& removed) {
    for (int i = 0; i < STATIONS; i++) {
        if (stations[i].removeOrder(orderId, removed)) {
            return true;
        }
    }
    return heap.removeOrder(orderId, removed);
}

void KitchenManager::clear() {
    heap.clear();
    for (int i = 0; i < STATIONS; i++) {
        stations[i].clear();
    }
}

int KitchenManager::pendingOrders() const {
    return heap.size();
}

int KitchenManager::stationQueueSize(int station) const {
    if (station < 0 || station >= STATIONS) {
        return 0;
    }
    return stations[station].size();
}

int KitchenManager::stationCount() const {
    return STATIONS;
}

void KitchenManager::showStatus() const {
    std::cout << "Waiting in heap: " << heap.size() << "\n";
    for (int i = 0; i < STATIONS; i++) {
        std::cout << "Cook " << i + 1 << " queue: ";
        stations[i].print();
        std::cout << "\n";
    }
}
