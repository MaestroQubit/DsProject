#pragma once

struct AnalyticsData
{
    int totalRestaurants;
    int totalRiders;
    int availableRiders;
    int busyRiders;
    int offlineRiders;

    double averageRating;
    double highestRating;

    AnalyticsData()
    {
        totalRestaurants = 0;
        totalRiders = 0;
        availableRiders = 0;
        busyRiders = 0;
        offlineRiders = 0;
        averageRating = 0.0;
        highestRating = 0.0;
    }
};