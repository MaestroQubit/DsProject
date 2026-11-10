#pragma once

#include <string>

struct Order
{
    int orderId;
    int restaurantId;
    std::string restaurantName;

    int customerLocationId;
    int assignedRiderId;

    int priority;
    int prepTime;

    double estimatedTravelTime;

    bool prepared;
    bool riderAssigned;
    bool delivered;

    Order()
    {
        orderId = -1;
        restaurantId = -1;
        restaurantName = "";

        customerLocationId = -1;
        assignedRiderId = -1;

        priority = 1;
        prepTime = 0;

        estimatedTravelTime = 0.0;

        prepared = false;
        riderAssigned = false;
        delivered = false;
    }

    Order(int id, int restaurant, const std::string &name, int customerLocation, int orderPriority = 1, int preparationTime = 0)
    {
        orderId = id;
        restaurantId = restaurant;
        restaurantName = name;

        customerLocationId = customerLocation;
        assignedRiderId = -1;

        priority = orderPriority;
        prepTime = preparationTime;

        estimatedTravelTime = 0.0;

        prepared = false;
        riderAssigned = false;
        delivered = false;
    }
};