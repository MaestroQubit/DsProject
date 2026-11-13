#include "../../include/dispatch/HashTable.h"

#include <algorithm>

HashTable::HashTable(std::size_t initialCapacity)
    : buckets(nextPrime(initialCapacity < 3 ? 3 : initialCapacity)),
      elementCount(0) {
    std::fill(buckets.begin(), buckets.end(), nullptr);
}

HashTable::~HashTable() {
    clear();
}

void HashTable::clear() {
    for (std::size_t i = 0; i < buckets.size(); ++i) {
        Node* current = buckets[i];
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        buckets[i] = nullptr;
    }
    elementCount = 0;
}

std::size_t HashTable::bucketIndex(int id) const {
    // Convert before modulo so negative IDs cannot produce a negative index.
    const long long value = static_cast<long long>(id);
    const unsigned long long magnitude = value < 0
        ? static_cast<unsigned long long>(-value)
        : static_cast<unsigned long long>(value);
    return static_cast<std::size_t>(magnitude % buckets.size());
}

bool HashTable::insertRider(const Rider& rider) {
    const std::size_t index = bucketIndex(rider.id);
    for (Node* current = buckets[index]; current != nullptr; current = current->next) {
        if (current->rider.id == rider.id) {
            return false;
        }
    }

    Node* node = new Node(rider);
    node->next = buckets[index];
    buckets[index] = node;
    ++elementCount;

    if (elementCount > buckets.size() * 3 / 4) {
        rehash(nextPrime(buckets.size() * 2 + 1));
    }
    return true;
}

Rider* HashTable::searchRider(int riderId) {
    const std::size_t index = bucketIndex(riderId);
    for (Node* current = buckets[index]; current != nullptr; current = current->next) {
        if (current->rider.id == riderId) {
            return &current->rider;
        }
    }
    return nullptr;
}

const Rider* HashTable::searchRider(int riderId) const {
    const std::size_t index = bucketIndex(riderId);
    for (const Node* current = buckets[index]; current != nullptr; current = current->next) {
        if (current->rider.id == riderId) {
            return &current->rider;
        }
    }
    return nullptr;
}

bool HashTable::updateRider(const Rider& rider) {
    Rider* existing = searchRider(rider.id);
    if (existing == nullptr) {
        return false;
    }
    *existing = rider;
    return true;
}

bool HashTable::deleteRider(int riderId) {
    const std::size_t index = bucketIndex(riderId);
    Node* current = buckets[index];
    Node* previous = nullptr;
    while (current != nullptr) {
        if (current->rider.id == riderId) {
            if (previous == nullptr) {
                buckets[index] = current->next;
            } else {
                previous->next = current->next;
            }
            delete current;
            --elementCount;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

std::vector<Rider> HashTable::getAllRiders() const {
    std::vector<Rider> result;
    result.reserve(elementCount);
    for (std::size_t i = 0; i < buckets.size(); ++i) {
        for (const Node* current = buckets[i]; current != nullptr; current = current->next) {
            result.push_back(current->rider);
        }
    }
    return result;
}

bool HashTable::isPrime(std::size_t value) {
    if (value < 2) return false;
    if (value % 2 == 0) return value == 2;
    for (std::size_t divisor = 3; divisor <= value / divisor; divisor += 2) {
        if (value % divisor == 0) return false;
    }
    return true;
}

std::size_t HashTable::nextPrime(std::size_t value) {
    while (!isPrime(value)) ++value;
    return value;
}

void HashTable::rehash(std::size_t newCapacity) {
    std::vector<Node*> newBuckets(nextPrime(newCapacity), nullptr);
    for (std::size_t i = 0; i < buckets.size(); ++i) {
        Node* current = buckets[i];
        while (current != nullptr) {
            Node* next = current->next;
            const long long id = static_cast<long long>(current->rider.id);
            const unsigned long long magnitude = id < 0
                ? static_cast<unsigned long long>(-id)
                : static_cast<unsigned long long>(id);
            const std::size_t index = static_cast<std::size_t>(magnitude % newBuckets.size());
            current->next = newBuckets[index];
            newBuckets[index] = current;
            current = next;
        }
    }
    buckets.swap(newBuckets);
}
