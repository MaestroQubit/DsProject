#include "../../include/system/FoodDeliverySystem.h"
#include "../../include/routing/CityData.h"
#include <algorithm>
#include <cmath>
#include <vector>
using namespace std;

FoodDeliverySystem::FoodDeliverySystem() : cityGraph(), routeManager(cityGraph)
{
    nextOrderId = 1;
    storageManager.initialize();
    buildCityMap(cityGraph);
    loadRestaurants("data/restaurants.csv");

    for (const Rider &rider : storageManager.getAllRiders())
        riderManager.insertRider(rider);

    for (const Order &order : storageManager.getAllOrders())
    {
        if (addOrder(order))
            nextOrderId = max(nextOrderId, order.orderId + 1);
    }

    deliveries = storageManager.getAllDeliveries();
}

SearchManager &FoodDeliverySystem::getSearchManager()
{
    return searchManager;
}

const SearchManager &FoodDeliverySystem::getSearchManager() const
{
    return searchManager;
}

Graph &FoodDeliverySystem::getGraph()
{
    return cityGraph;
}

const Graph &FoodDeliverySystem::getGraph() const
{
    return cityGraph;
}

RouteManager &FoodDeliverySystem::getRouteManager()
{
    return routeManager;
}

const RouteManager &FoodDeliverySystem::getRouteManager() const
{
    return routeManager;
}

RiderManager &FoodDeliverySystem::getRiderManager()
{
    return riderManager;
}

const RiderManager &FoodDeliverySystem::getRiderManager() const
{
    return riderManager;
}

KitchenManager &FoodDeliverySystem::getKitchenManager()
{
    return kitchenManager;
}

const KitchenManager &FoodDeliverySystem::getKitchenManager() const
{
    return kitchenManager;
}

AnalyticsManager &FoodDeliverySystem::getAnalyticsManager()
{
    return analyticsManager;
}

const AnalyticsManager &FoodDeliverySystem::getAnalyticsManager() const
{
    return analyticsManager;
}

AnalyticsData FoodDeliverySystem::getAnalyticsReport()
{
    return analyticsManager.generateReport(searchManager, riderManager);
}

StorageManager &FoodDeliverySystem::getStorageManager()
{
    return storageManager;
}

const StorageManager &FoodDeliverySystem::getStorageManager() const
{
    return storageManager;
}

Order *FoodDeliverySystem::getOrderById(int orderId)
{
    for (Order &order : orders)
    {
        if (order.orderId == orderId)
            return &order;
    }
    return nullptr;
}

const Order *FoodDeliverySystem::getOrderById(int orderId) const
{
    for (const Order &order : orders)
    {
        if (order.orderId == orderId)
            return &order;
    }
    return nullptr;
}

vector<Order> &FoodDeliverySystem::getOrders()
{
    return orders;
}

const vector<Order> &FoodDeliverySystem::getOrders() const
{
    return orders;
}

Delivery *FoodDeliverySystem::getDeliveryByOrderId(int orderId)
{
    for (Delivery &delivery : deliveries)
    {
        if (delivery.orderId == orderId)
            return &delivery;
    }
    return nullptr;
}

const Delivery *FoodDeliverySystem::getDeliveryByOrderId(int orderId) const
{
    for (const Delivery &delivery : deliveries)
    {
        if (delivery.orderId == orderId)
            return &delivery;
    }
    return nullptr;
}

int FoodDeliverySystem::generateOrderId()
{
    return nextOrderId++;
}

bool FoodDeliverySystem::addOrder(const Order &order)
{
    if (order.orderId < 0 || order.restaurantId < 0 ||
        order.customerLocationId < 0 || order.priority < 0 ||
        order.customerLocationId >= cityGraph.getVertexCount() ||
        order.prepTime < 0 || !isfinite(order.estimatedTravelTime) ||
        order.estimatedTravelTime < 0.0 ||
        getOrderById(order.orderId) != nullptr ||
        searchManager.getRestaurantById(order.restaurantId) == nullptr)
        return false;

    orders.push_back(order);
    kitchenManager.addOrder(order.orderId, order.restaurantId, order.prepTime,
                            order.priority);
    return true;
}

int FoodDeliverySystem::getOrderCount() const
{
    return static_cast<int>(orders.size());
}

bool FoodDeliverySystem::loadRestaurants(const string &filename)
{
    return searchManager.loadRestaurant(filename);
}

int FoodDeliverySystem::addLocation(const string &locationName)
{
    return cityGraph.addVertex(locationName);
}

bool FoodDeliverySystem::addRoad(int source, int destination, double travelTime)
{
    return cityGraph.addEdge(source, destination, travelTime);
}

int FoodDeliverySystem::createOrder(int restaurantId, int customerLocationId, int priority, int prepTime)
{
    const Restaurant *restaurant = searchManager.getRestaurantById(restaurantId);
    if (restaurant == nullptr || customerLocationId < 0 ||
        customerLocationId >= cityGraph.getVertexCount() || priority < 0 ||
        prepTime < 0)
        return -1;
    int id = generateOrderId();
    Order order(id, restaurant->id, restaurant->name, customerLocationId, priority, prepTime);
    orders.push_back(order);
    kitchenManager.addOrder(order.orderId, order.restaurantId, order.prepTime, order.priority);
    return id;
}

bool FoodDeliverySystem::markOrderPrepared(int orderId)
{
    Order *order = getOrderById(orderId);
    if (order == nullptr || order->delivered)
        return false;
    order->prepared = true;
    return true;
}

bool FoodDeliverySystem::assignRiderToOrder(int orderId, const vector<RiderDistance> &distances)
{
    Order *order =
        getOrderById(orderId);
    if (order == nullptr || order->delivered || !order->prepared ||
        order->riderAssigned)
        return false;
    Rider rider;
    if (!riderManager.assignBestRider(orderId, distances, rider))
        return false;
    order->assignedRiderId = rider.id;
    order->riderAssigned = true;
    return true;
}

bool FoodDeliverySystem::completeOrder(int orderId)
{
    Order *order =
        getOrderById(orderId);
    if (order == nullptr || order->delivered || !order->riderAssigned)
        return false;

    Delivery *delivery = getDeliveryByOrderId(orderId);
    if (delivery == nullptr)
        return false;

    if (!riderManager.completeDelivery(order->assignedRiderId))
        return false;

    delivery->completed = true;
    order->delivered = true;
    return true;
}

bool FoodDeliverySystem::calculateRouteForOrder(int orderId, RouteResult &result)
{
    Order *order =
        getOrderById(orderId);
    if (order == nullptr)
        return false;
    const Restaurant *restaurant = searchManager.getRestaurantById(order->restaurantId);
    if (restaurant == nullptr)
        return false;
    int source = restaurant->locationId;
    int destination = order->customerLocationId;
    result = routeManager.shortestPath(source, destination);
    if (!result.routeExists)
        return false;
    order->estimatedTravelTime = result.totalTravelTime;
    return true;
}

bool FoodDeliverySystem::createDelivery(int orderId, int riderId, double travelTime)
{
    Order *order =
        getOrderById(orderId);
    Rider rider;
    if (order == nullptr || order->delivered || !order->riderAssigned ||
        order->assignedRiderId != riderId || !isfinite(travelTime) ||
        travelTime < 0.0 || getDeliveryByOrderId(orderId) != nullptr ||
        !riderManager.searchRider(riderId, rider) || rider.status != "Busy")
        return false;
    deliveries.push_back(
        Delivery(orderId, riderId, travelTime));
    return true;
}

bool FoodDeliverySystem::setRestaurantLocation(int restaurantId, int locationId)
{
    return searchManager.setRestaurantLocation(restaurantId, locationId);
}

int FoodDeliverySystem::getRestaurantLocation(int restaurantId) const
{
    return searchManager.getRestaurantLocation(restaurantId);
}

bool FoodDeliverySystem::saveRestaurant(const Restaurant &restaurant)
{
    return storageManager.insertRestaurant(restaurant);
}

bool FoodDeliverySystem::saveOrder(const Order &order)
{
    return storageManager.insertOrder(order);
}

bool FoodDeliverySystem::saveDelivery(const Delivery &delivery)
{
    return storageManager.insertDelivery(delivery);
}

bool FoodDeliverySystem::saveRider(const Rider &rider)
{
    return storageManager.insertRider(rider);
}