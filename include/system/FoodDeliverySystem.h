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
    bool initialized;
    std::string currentDataDir;

public:
    explicit FoodDeliverySystem(const std::string &dataDir = "");

    FoodDeliverySystem(const FoodDeliverySystem &) = delete;
    FoodDeliverySystem &operator=(const FoodDeliverySystem &) = delete;

    bool initialize(const std::string &dataDir = "");
    bool isInitialized() const;

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
    bool loadRiders(const std::string &filename);
    int addLocation(const std::string &locationName, int externalId = -1);
    bool addRoad(int source, int destination, double travelTime);
    int createOrder(int restaurantId, int customerLocationId, int priority, int prepTime);
    bool markOrderPrepared(int orderId);
    bool completeKitchenOrder(int station, KitchenOrder &readyOrder);
    bool assignRiderToOrder(int orderId, const std::vector<RiderDistance> &distances = {});
    bool completeOrder(int orderId);
    bool calculateRouteForOrder(int orderId, RouteResult &result);
    bool createDelivery(int orderId, int riderId, double travelTime);
    bool setRestaurantLocation(int restaurantId, int locationId);
    int getRestaurantLocation(int restaurantId) const;
    bool saveRestaurant(const Restaurant &restaurant);
    bool saveOrder(const Order &order);
    bool saveDelivery(const Delivery &delivery);
    bool saveRider(const Rider &rider);

    // System-level coordinated operations for UI and clients
    bool addRestaurant(const std::string &name, int locationId);
    bool addRider(const Rider &rider);
    bool updateRider(const Rider &rider);
    bool updateRiderStatus(int riderId, const std::string &status);
    bool updateRoadTraffic(int source, int destination, double travelTime);
    bool closeRoad(int source, int destination);
    bool reopenRoad(int source, int destination);
    bool addMenuItem(const MenuItem &item);
    std::vector<MenuItem> getAllMenuItems();
    std::vector<Restaurant> getAllRestaurants() const;
    std::vector<Rider> getAllRiders() const;
    std::vector<Rider> getAvailableRiders() const;
    std::vector<std::string> searchRestaurants(const std::string &query) const;
    RouteResult findShortestRoute(int sourceLocationId, int destLocationId) const;
};