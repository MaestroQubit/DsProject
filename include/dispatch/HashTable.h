#pragma once

#include "../models/Rider.h"
#include <cstddef>
#include <string>
#include <vector>

class HashTable {
private:
    struct Node {
        Rider rider;
        Node* next;
        explicit Node(const Rider& value) : rider(value), next(nullptr) {}
    };

    std::vector<Node*> buckets;
    std::size_t elementCount;

    std::size_t bucketIndex(int id) const;
    void rehash(std::size_t newCapacity);
    static bool isPrime(std::size_t value);
    static std::size_t nextPrime(std::size_t value);

public:
    explicit HashTable(std::size_t initialCapacity = 17);
    ~HashTable();

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    bool insertRider(const Rider& rider);
    Rider* searchRider(int riderId);
    const Rider* searchRider(int riderId) const;
    bool updateRider(const Rider& rider);
    bool deleteRider(int riderId);

    std::size_t size() const { return elementCount; }
    bool empty() const { return elementCount == 0; }
    std::vector<Rider> getAllRiders() const;
};
