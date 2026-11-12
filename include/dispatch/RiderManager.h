#pragma once

#include "HashTable.h"
#include "MaxHeap.h"
#include <string>
#include <vector>

struct RiderDistance {
    int riderId;
    double distance;
    RiderDistance(int id, double value) : riderId(id), distance(value) {}
};

class RiderManager {
private:
    HashTable riders;

    static bool isValidRating(double rating);
    static bool normalizeStatus(const std::string& value, std::string& normalized);

public:
    explicit RiderManager(std::size_t initialCapacity = 17) : riders(initialCapacity) {}

    bool insertRider(const Rider& rider);
    bool searchRider(int riderId, Rider& result) const;
    bool updateRider(const Rider& rider);
    bool deleteRider(int riderId);
    bool updateRiderStatus(int riderId, const std::string& status);

    std::vector<Rider> getAllRiders() const;
    std::vector<Rider> getAvailableRiders() const;

    // Returns candidates from highest to lowest score. Score = rating * 10 - distance.
    bool rankAvailableRiders(const std::vector<RiderDistance>& distances,
                             std::vector<RiderCandidate>& ranked) const;

    // Assigns the best available rider and marks them Busy. Returns false if
    // the order ID is invalid or no supplied available rider can be matched.
    bool assignBestRider(int orderId, const std::vector<RiderDistance>& distances,
                         Rider& assignedRider);

    // Mark a currently assigned rider Available after delivery/cancellation.
    bool completeDelivery(int riderId);
};
