#pragma once

#include "../search/SearchManager.h"
#include "../routing/Graph.h"
#include "../routing/RouteManager.h"
#include "../dispatch/RiderManager.h"
#include "../kitchen/KitchenManager.h"
#include "../analytics/AnalyticsManager.h"
#include "../analytics/AnalyticsData.h"
#include "../storage/StorageManager.h"
#include "../models/Order.h"
#include "../models/Delivery.h"

#include <vector>
#include <string>

class FoodDeliverySystem
{
private:
    SearchManager searchManager;
    RiderManager riderManager;
    KitchenManager kitchenManager;
    AnalyticsManager analyticsManager;
    StorageManager storageManager;
    Graph cityGraph;
    RouteManager routeManager;
    std::vector<Order> orders;
    std::vector<Delivery> deliveries;

    int nextOrderId;

public:
    FoodDeliverySystem();

    FoodDeliverySystem(const FoodDeliverySystem &) = delete;
    FoodDeliverySystem &operator=(const FoodDeliverySystem &) = delete;

    SearchManager &getSearchManager();
    const SearchManager &getSearchManager() const;

    Graph &getGraph();
    const Graph &getGraph() const;

    RouteManager &getRouteManager();
    const RouteManager &getRouteManager() const;

    RiderManager &getRiderManager();
    const RiderManager &getRiderManager() const;

    KitchenManager &getKitchenManager();
    const KitchenManager &getKitchenManager() const;

    AnalyticsManager &getAnalyticsManager();
    const AnalyticsManager &getAnalyticsManager() const;

    StorageManager &getStorageManager();
    const StorageManager &getStorageManager() const;

    Order *getOrderById(int orderId);
    std::vector<Order> &getOrders();
    const Order *getOrderById(int orderId) const;
    const std::vector<Order> &getOrders() const;

    Delivery *getDeliveryByOrderId(int orderId);
    const Delivery *getDeliveryByOrderId(int orderId) const;

    AnalyticsData getAnalyticsReport();

    int generateOrderId();
    bool addOrder(const Order &order);
    int getOrderCount() const;
    bool loadRestaurants(const std::string &filename);
    int addLocation(const std::string &locationName);
    bool addRoad(int source, int destination, double travelTime);
    int createOrder(int restaurantId, int customerLocationId, int priority, int prepTime);
    bool markOrderPrepared(int orderId);
    bool assignRiderToOrder(int orderId, const std::vector<RiderDistance> &distances);
    bool completeOrder(int orderId);
    bool calculateRouteForOrder(int orderId, RouteResult &result);
    bool createDelivery(int orderId, int riderId, double travelTime);
    bool setRestaurantLocation(int restaurantId, int locationId);
    int getRestaurantLocation(int restaurantId) const;
    bool saveRestaurant(const Restaurant &restaurant);
    bool saveOrder(const Order &order);
    bool saveDelivery(const Delivery &delivery);
    bool saveRider(const Rider &rider);
};