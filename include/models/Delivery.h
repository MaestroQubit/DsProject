#pragma once

struct Delivery
{
    int orderId;
    int riderId;

    double travelTime;

    bool completed;

    Delivery()
    {
        orderId = -1;
        riderId = -1;

        travelTime = 0.0;

        completed = false;
    }

    Delivery(int order, int rider, double time)
    {
        orderId = order;
        riderId = rider;

        travelTime = time;

        completed = false;
    }
};