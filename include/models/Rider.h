#pragma once

#include <string>

struct Rider
{
    int id;
    std::string name;
    double rating;
    std::string status;
    int assignedOrderId;

    Rider()
    {
        id = -1;
        rating = 0.0;
        status = "Available";
        assignedOrderId = -1;
    }

    Rider(int riderId, const std::string &riderName, double riderRating, const std::string &riderStatus = "Available")
    {
        id = riderId;
        name = riderName;
        rating = riderRating;
        status = riderStatus;
        assignedOrderId = -1;
    }
};