#include "../../include/analytics/AnalyticsManager.h"

using namespace std;

AnalyticsManager::AnalyticsManager()
{
    riderCount = 0;
}

void AnalyticsManager::buildAnalytics(const RiderManager &riderManager)
{
    vector<Rider> riders = riderManager.getAllRiders();
    vector<double> ratings;
    for (const Rider &rider : riders)
        ratings.push_back(rider.rating);
    riderCount = ratings.size();
    ratingTree.buildTree(ratings);
}

int AnalyticsManager::getTotalRestaurants(const SearchManager &searchManager) const
{
    return searchManager.getRestaurantCount();
}

int AnalyticsManager::getTotalRiders() const
{
    return riderCount;
}

double AnalyticsManager::getAverageRating() const
{
    if (riderCount == 0)
        return 0.0;
    return ratingTree.rangeSum(0, riderCount - 1) / riderCount;
}

double AnalyticsManager::getHighestRating() const
{
    if (riderCount == 0)
        return 0.0;
    return ratingTree.rangeMax(0, riderCount - 1);
}

AnalyticsData AnalyticsManager::generateReport(const SearchManager &searchManager, const RiderManager &riderManager)
{
    buildAnalytics(riderManager);
    AnalyticsData data;
    data.totalRestaurants = searchManager.getRestaurantCount();
    vector<Rider> riders = riderManager.getAllRiders();
    data.totalRiders = riders.size();
    for (const Rider &rider : riders)
    {
        if (rider.status == "Available")
            data.availableRiders++;
        else if (rider.status == "Busy")
            data.busyRiders++;
        else if (rider.status == "Offline")
            data.offlineRiders++;
    }
    data.averageRating = getAverageRating();
    data.highestRating = getHighestRating();
    return data;
}