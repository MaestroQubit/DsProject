#pragma once

#include <cstddef>
#include <string>
#include <vector>

// Rider state shared by the dispatch hash table and manager.
struct Rider {
    int id;
    std::string name;
    double rating;
    std::string status;
    int assignedOrderId;

    Rider()
        : id(-1), rating(0.0), status("Available"), assignedOrderId(-1) {}

    Rider(int riderId, const std::string& riderName, double riderRating,
          const std::string& riderStatus = "Available")
        : id(riderId), name(riderName), rating(riderRating),
          status(riderStatus), assignedOrderId(-1) {}
};

// Separate-chaining hash table keyed by rider ID.
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
