#pragma once

#include "HashTable.h"
#include <cstddef>
#include <vector>

struct RiderCandidate {
    Rider rider;
    double distance;
    double score;

    RiderCandidate() : distance(0.0), score(0.0) {}
    RiderCandidate(const Rider& value, double deliveryDistance, double rankingScore)
        : rider(value), distance(deliveryDistance), score(rankingScore) {}
};

// Max-heap ordered by dispatch score. Ties are resolved by shorter distance,
// then lower rider ID to keep dispatch deterministic.
class MaxHeap {
private:
    std::vector<RiderCandidate> heap;

    static bool higherPriority(const RiderCandidate& lhs,
                               const RiderCandidate& rhs);
    void siftUp(std::size_t index);
    void siftDown(std::size_t index);

public:
    MaxHeap() = default;

    void insert(const RiderCandidate& candidate);
    bool extractMax(RiderCandidate& candidate);
    bool peek(RiderCandidate& candidate) const;
    void clear() { heap.clear(); }
    bool empty() const { return heap.empty(); }
    std::size_t size() const { return heap.size(); }

    static double calculateScore(double rating, double distance);
};
