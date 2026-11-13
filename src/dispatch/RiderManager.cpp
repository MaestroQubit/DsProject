#include "../../include/dispatch/RiderManager.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>

bool RiderManager::isValidRating(double rating) {
    return std::isfinite(rating) && rating >= 0.0 && rating <= 5.0;
}

bool RiderManager::normalizeStatus(const std::string& value, std::string& normalized) {
    std::string lower;
    lower.reserve(value.size());
    for (std::size_t i = 0; i < value.size(); ++i) {
        lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(value[i]))));
    }

    if (lower == "available") normalized = "Available";
    else if (lower == "busy" || lower == "on delivery") normalized = "Busy";
    else if (lower == "offline") normalized = "Offline";
    else return false;
    return true;
}

void RiderManager::clear() {
    riders.clear();
}

bool RiderManager::insertRider(const Rider& rider) {
    std::string status;
    if (rider.id < 0 || rider.name.empty() || !isValidRating(rider.rating) ||
        !normalizeStatus(rider.status, status)) {
        return false;
    }
    Rider normalized = rider;
    normalized.status = status;
    if (normalized.status != "Busy") normalized.assignedOrderId = -1;
    return riders.insertRider(normalized);
}

bool RiderManager::searchRider(int riderId, Rider& result) const {
    const Rider* found = riders.searchRider(riderId);
    if (found == nullptr) return false;
    result = *found;
    return true;
}

bool RiderManager::updateRider(const Rider& rider) {
    std::string status;
    if (rider.id < 0 || rider.name.empty() || !isValidRating(rider.rating) ||
        !normalizeStatus(rider.status, status) || riders.searchRider(rider.id) == nullptr) {
        return false;
    }
    Rider normalized = rider;
    normalized.status = status;
    if (normalized.status != "Busy") normalized.assignedOrderId = -1;
    return riders.updateRider(normalized);
}

bool RiderManager::deleteRider(int riderId) {
    return riders.deleteRider(riderId);
}

bool RiderManager::updateRiderStatus(int riderId, const std::string& status) {
    Rider* rider = riders.searchRider(riderId);
    std::string normalized;
    if (rider == nullptr || !normalizeStatus(status, normalized)) return false;
    rider->status = normalized;
    if (normalized != "Busy") rider->assignedOrderId = -1;
    return true;
}

std::vector<Rider> RiderManager::getAllRiders() const {
    return riders.getAllRiders();
}

std::vector<Rider> RiderManager::getAvailableRiders() const {
    std::vector<Rider> result;
    const std::vector<Rider> all = riders.getAllRiders();
    for (std::size_t i = 0; i < all.size(); ++i) {
        if (all[i].status == "Available") result.push_back(all[i]);
    }
    return result;
}

bool RiderManager::rankAvailableRiders(const std::vector<RiderDistance>& distances,
                                       std::vector<RiderCandidate>& ranked) const {
    ranked.clear();
    MaxHeap heap;
    std::vector<int> seenIds;
    for (std::size_t i = 0; i < distances.size(); ++i) {
        const RiderDistance& distance = distances[i];
        if (distance.riderId < 0 || !std::isfinite(distance.distance) || distance.distance < 0.0 ||
            std::find(seenIds.begin(), seenIds.end(), distance.riderId) != seenIds.end()) {
            continue;
        }
        seenIds.push_back(distance.riderId);
        const Rider* rider = riders.searchRider(distance.riderId);
        if (rider == nullptr || rider->status != "Available") continue;
        const double score = MaxHeap::calculateScore(rider->rating, distance.distance);
        heap.insert(RiderCandidate(*rider, distance.distance, score));
    }

    RiderCandidate candidate;
    while (heap.extractMax(candidate)) ranked.push_back(candidate);
    return !ranked.empty();
}

bool RiderManager::assignBestRider(int orderId, const std::vector<RiderDistance>& distances,
                                   Rider& assignedRider) {
    if (orderId < 0) return false;
    std::vector<RiderCandidate> ranked;
    if (!rankAvailableRiders(distances, ranked)) return false;

    Rider* selected = riders.searchRider(ranked[0].rider.id);
    // Recheck availability in case the manager was changed between ranking and assignment.
    if (selected == nullptr || selected->status != "Available") return false;
    selected->status = "Busy";
    selected->assignedOrderId = orderId;
    assignedRider = *selected;
    return true;
}

bool RiderManager::completeDelivery(int riderId) {
    Rider* rider = riders.searchRider(riderId);
    if (rider == nullptr || rider->status != "Busy") return false;
    rider->status = "Available";
    rider->assignedOrderId = -1;
    return true;
}
