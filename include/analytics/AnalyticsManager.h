#pragma once

#include "../dispatch/RiderManager.h"
#include "../search/SearchManager.h"
#include "AnalyticsData.h"
#include "SegmentTree.h"

class AnalyticsManager
{
    SegmentTree ratingTree;
    int riderCount;

public:
    AnalyticsManager();

    void buildAnalytics(const RiderManager &riderManager);
    AnalyticsData generateReport(const SearchManager &searchManager, const RiderManager &riderManager);
    int getTotalRestaurants(const SearchManager &searchManager) const;
    int getTotalRiders() const;
    double getAverageRating() const;
    double getHighestRating() const;
};