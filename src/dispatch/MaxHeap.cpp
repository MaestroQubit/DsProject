#include "../../include/dispatch/MaxHeap.h"

#include <cmath>
#include <limits>

bool MaxHeap::higherPriority(const RiderCandidate& lhs,
                             const RiderCandidate& rhs) {
    if (lhs.score != rhs.score) return lhs.score > rhs.score;
    if (lhs.distance != rhs.distance) return lhs.distance < rhs.distance;
    return lhs.rider.id < rhs.rider.id;
}

void MaxHeap::siftUp(std::size_t index) {
    while (index > 0) {
        const std::size_t parent = (index - 1) / 2;
        if (!higherPriority(heap[index], heap[parent])) break;
        std::swap(heap[index], heap[parent]);
        index = parent;
    }
}

void MaxHeap::siftDown(std::size_t index) {
    while (true) {
        const std::size_t left = index * 2 + 1;
        const std::size_t right = left + 1;
        std::size_t best = index;
        if (left < heap.size() && higherPriority(heap[left], heap[best])) best = left;
        if (right < heap.size() && higherPriority(heap[right], heap[best])) best = right;
        if (best == index) break;
        std::swap(heap[index], heap[best]);
        index = best;
    }
}

void MaxHeap::insert(const RiderCandidate& candidate) {
    heap.push_back(candidate);
    siftUp(heap.size() - 1);
}

bool MaxHeap::extractMax(RiderCandidate& candidate) {
    if (heap.empty()) return false;
    candidate = heap.front();
    if (heap.size() == 1) {
        heap.pop_back();
        return true;
    }
    heap.front() = heap.back();
    heap.pop_back();
    siftDown(0);
    return true;
}

bool MaxHeap::peek(RiderCandidate& candidate) const {
    if (heap.empty()) return false;
    candidate = heap.front();
    return true;
}

double MaxHeap::calculateScore(double rating, double distance) {
    if (!std::isfinite(rating) || !std::isfinite(distance) || distance < 0.0) {
        return -std::numeric_limits<double>::infinity();
    }
    return rating * 10.0 - distance;
}
