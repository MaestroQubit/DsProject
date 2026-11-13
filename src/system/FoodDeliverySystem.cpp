#include "../../include/system/FoodDeliverySystem.h"
#include "../../include/routing/CityData.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace std;

static string resolveDirectory(const string &dir)
{
    if (!dir.empty() && filesystem::exists(dir))
        return dir;
    const char *candidates[] = {"data", "../data", "../../data", "../../../data"};
    for (const char *cand : candidates)
    {
        if (filesystem::exists(cand) && filesystem::is_directory(cand))
        {
            if (filesystem::exists(string(cand) + "/locations.csv"))
                return cand;
        }
    }
    return dir.empty() ? "data" : dir;
}

static string resolveFilePath(const string &path)
{
    if (path.empty())
        return path;
    if (filesystem::exists(path))
        return path;
    string p1 = "../" + path;
    if (filesystem::exists(p1))
        return p1;
    string p2 = "../../" + path;
    if (filesystem::exists(p2))
        return p2;
    string p3 = "../../../" + path;
    if (filesystem::exists(p3))
        return p3;
    return path;
}

FoodDeliverySystem::FoodDeliverySystem(const string &dataDir)
    : cityGraph(), routeManager(cityGraph), nextOrderId(1), initialized(false)
{
    initialize(dataDir);
}

bool FoodDeliverySystem::initialize(const string &dataDir)
{
    cityGraph.clear();
    searchManager.clear();
    riderManager.clear();
    kitchenManager.clear();
    orders.clear();
    deliveries.clear();
    nextOrderId = 1;

    currentDataDir = resolveDirectory(dataDir);
    storageManager.setDataFolder(currentDataDir);
    bool ok = storageManager.initialize();

    if (!buildCityMap(cityGraph, currentDataDir))
        ok = false;

    if (!loadRestaurants(currentDataDir + "/restaurants.csv"))
        ok = false;

    vector<Restaurant> storedRestaurants = storageManager.getAllRestaurants();
    if (storedRestaurants.empty())
    {
        for (const Restaurant &r : searchManager.getAllRestaurants())
        {
            storageManager.insertRestaurant(r);
        }
    }
    else
    {
        for (const Restaurant &sr : storedRestaurants)
        {
            searchManager.setRestaurantLocation(sr.id, sr.locationId);
        }
    }

    vector<Rider> storedRiders = storageManager.getAllRiders();
    if (storedRiders.empty())
    {
        if (!loadRiders(currentDataDir + "/riders.csv"))
            ok = false;
    }
    else
    {
        for (const Rider &rider : storedRiders)
            riderManager.insertRider(rider);
    }

    orders.clear();
    nextOrderId = 1;
    for (const Order &order : storageManager.getAllOrders())
    {
        orders.push_back(order);
        nextOrderId = max(nextOrderId, order.orderId + 1);
        if (!order.prepared && !order.delivered)
        {
            kitchenManager.addOrder(order.orderId, order.restaurantId, order.prepTime, order.priority);
        }
    }
    kitchenManager.sendToStations();

    deliveries = storageManager.getAllDeliveries();
    initialized = ok;
    return initialized;
}

bool FoodDeliverySystem::isInitialized() const
{
    return initialized;
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
        cityGraph.getVertexIndex(order.customerLocationId) < 0 ||
        order.prepTime < 0 || !isfinite(order.estimatedTravelTime) ||
        order.estimatedTravelTime < 0.0 ||
        getOrderById(order.orderId) != nullptr ||
        searchManager.getRestaurantById(order.restaurantId) == nullptr)
        return false;

    orders.push_back(order);
    if (!order.prepared && !order.delivered)
    {
        kitchenManager.addOrder(order.orderId, order.restaurantId, order.prepTime, order.priority);
        kitchenManager.sendToStations();
    }
    return true;
}

int FoodDeliverySystem::getOrderCount() const
{
    return static_cast<int>(orders.size());
}

bool FoodDeliverySystem::loadRestaurants(const string &filename)
{
    string resolved = resolveFilePath(filename);
    return searchManager.loadRestaurant(resolved);
}

bool FoodDeliverySystem::loadRiders(const string &filename)
{
    string resolved = resolveFilePath(filename);
    ifstream file(resolved);
    if (!file)
        return false;

    string line;
    if (!getline(file, line))
        return false;

    while (getline(file, line))
    {
        if (line.empty())
            continue;
        if (line.back() == '\r')
            line.pop_back();
        if (line.empty())
            continue;

        stringstream ss(line);
        string idToken, name, phone, locToken, status;
        if (!getline(ss, idToken, ',') || !getline(ss, name, ',') ||
            !getline(ss, phone, ',') || !getline(ss, locToken, ',') ||
            !getline(ss, status, ','))
            continue;

        if (!status.empty() && status.back() == '\r')
            status.pop_back();

        int id = -1;
        try
        {
            if (!idToken.empty() && (idToken[0] == 'R' || idToken[0] == 'r'))
                id = stoi(idToken.substr(1));
            else
                id = stoi(idToken);
        }
        catch (...)
        {
            continue;
        }

        int locId = -1;
        try
        {
            locId = stoi(locToken);
        }
        catch (...)
        {
            locId = -1;
        }

        Rider rider(id, name, 4.5, status, locId);
        if (riderManager.insertRider(rider))
        {
            storageManager.insertRider(rider);
        }
    }
    return true;
}

int FoodDeliverySystem::addLocation(const string &locationName, int externalId)
{
    return cityGraph.addVertex(locationName, externalId);
}

bool FoodDeliverySystem::addRoad(int source, int destination, double travelTime)
{
    return cityGraph.addEdge(source, destination, travelTime);
}

int FoodDeliverySystem::createOrder(int restaurantId, int customerLocationId, int priority, int prepTime)
{
    const Restaurant *restaurant = searchManager.getRestaurantById(restaurantId);
    if (restaurant == nullptr || customerLocationId < 0 ||
        cityGraph.getVertexIndex(customerLocationId) < 0 || priority < 0 ||
        prepTime < 0)
        return -1;
    int id = generateOrderId();
    Order order(id, restaurant->id, restaurant->name, customerLocationId, priority, prepTime);

    if (!storageManager.insertOrder(order))
        return -1;

    orders.push_back(order);
    kitchenManager.addOrder(order.orderId, order.restaurantId, order.prepTime, order.priority);
    kitchenManager.sendToStations();
    return id;
}

bool FoodDeliverySystem::markOrderPrepared(int orderId)
{
    Order *order = getOrderById(orderId);
    if (order == nullptr || order->delivered || order->prepared)
        return false;

    KitchenOrder removed;
    if (!kitchenManager.removeOrder(orderId, removed))
        return false;

    order->prepared = true;
    if (storageManager.orderExists(orderId))
    {
        if (!storageManager.updateOrder(*order))
        {
            order->prepared = false;
            kitchenManager.addOrder(removed.orderId, removed.restaurantId, removed.prepTime, removed.priority);
            kitchenManager.sendToStations();
            return false;
        }
    }
    return true;
}

bool FoodDeliverySystem::completeKitchenOrder(int station, KitchenOrder &readyOrder)
{
    if (station < 0 || station >= kitchenManager.stationCount())
        return false;

    KitchenOrder peeked;
    if (!kitchenManager.peekStationOrder(station, peeked))
        return false;

    Order *order = getOrderById(peeked.orderId);
    if (order == nullptr || order->delivered || order->prepared)
        return false;

    if (!kitchenManager.completeOrder(station, readyOrder))
        return false;

    order->prepared = true;
    if (storageManager.orderExists(order->orderId))
    {
        if (!storageManager.updateOrder(*order))
        {
            order->prepared = false;
            return false;
        }
    }
    return true;
}

bool FoodDeliverySystem::assignRiderToOrder(int orderId, const vector<RiderDistance> &distances)
{
    Order *order = getOrderById(orderId);
    if (order == nullptr || order->delivered || !order->prepared || order->riderAssigned)
        return false;

    const Restaurant *restaurant = searchManager.getRestaurantById(order->restaurantId);
    if (restaurant == nullptr)
        return false;

    RouteResult deliveryRoute = routeManager.shortestPath(restaurant->locationId, order->customerLocationId);
    if (!deliveryRoute.routeExists)
        return false;

    vector<RiderDistance> candidateDistances = distances;
    if (candidateDistances.empty())
    {
        int restaurantLocation = restaurant->locationId;
        vector<Rider> availableRiders = riderManager.getAvailableRiders();
        for (const Rider &r : availableRiders)
        {
            if (r.locationId >= 0 && cityGraph.getVertexIndex(r.locationId) >= 0)
            {
                RouteResult rr = routeManager.shortestPath(r.locationId, restaurantLocation);
                if (rr.routeExists)
                {
                    candidateDistances.push_back(RiderDistance(r.id, rr.totalTravelTime));
                }
            }
        }
    }

    Rider rider;
    if (!riderManager.assignBestRider(orderId, candidateDistances, rider))
        return false;

    order->assignedRiderId = rider.id;
    order->riderAssigned = true;
    order->estimatedTravelTime = deliveryRoute.totalTravelTime;

    Delivery delivery(orderId, rider.id, deliveryRoute.totalTravelTime);

    if (!storageManager.updateOrder(*order))
    {
        order->assignedRiderId = -1;
        order->riderAssigned = false;
        riderManager.updateRiderStatus(rider.id, "Available");
        return false;
    }

    if (!storageManager.updateRider(rider))
    {
        order->assignedRiderId = -1;
        order->riderAssigned = false;
        storageManager.updateOrder(*order);
        riderManager.updateRiderStatus(rider.id, "Available");
        return false;
    }

    if (!storageManager.insertDelivery(delivery))
    {
        order->assignedRiderId = -1;
        order->riderAssigned = false;
        storageManager.updateOrder(*order);
        riderManager.updateRiderStatus(rider.id, "Available");
        Rider revRider;
        if (riderManager.searchRider(rider.id, revRider))
            storageManager.updateRider(revRider);
        return false;
    }

    deliveries.push_back(delivery);
    return true;
}

bool FoodDeliverySystem::completeOrder(int orderId)
{
    Order *order = getOrderById(orderId);
    if (order == nullptr || order->delivered || !order->prepared || !order->riderAssigned)
        return false;

    Delivery *delivery = getDeliveryByOrderId(orderId);
    if (delivery == nullptr || delivery->completed)
        return false;

    Rider assignedRider;
    if (!riderManager.searchRider(order->assignedRiderId, assignedRider))
        return false;

    Delivery oldDelivery = *delivery;
    Order oldOrder = *order;
    Rider oldRider = assignedRider;

    if (!riderManager.completeDelivery(order->assignedRiderId))
        return false;

    delivery->completed = true;
    order->delivered = true;
    assignedRider.status = "Available";
    assignedRider.locationId = order->customerLocationId;
    riderManager.updateRider(assignedRider);

    if (!storageManager.updateDelivery(*delivery) ||
        !storageManager.updateOrder(*order) ||
        !storageManager.updateRider(assignedRider))
    {
        *delivery = oldDelivery;
        *order = oldOrder;
        riderManager.updateRider(oldRider);
        storageManager.updateDelivery(oldDelivery);
        storageManager.updateOrder(oldOrder);
        storageManager.updateRider(oldRider);
        return false;
    }

    return true;
}

bool FoodDeliverySystem::calculateRouteForOrder(int orderId, RouteResult &result)
{
    Order *order = getOrderById(orderId);
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

    double oldTime = order->estimatedTravelTime;
    order->estimatedTravelTime = result.totalTravelTime;
    if (storageManager.orderExists(orderId))
    {
        if (!storageManager.updateOrder(*order))
        {
            order->estimatedTravelTime = oldTime;
            return false;
        }
    }
    return true;
}

bool FoodDeliverySystem::createDelivery(int orderId, int riderId, double travelTime)
{
    Order *order = getOrderById(orderId);
    Rider rider;
    if (order == nullptr || order->delivered || !order->prepared || !order->riderAssigned ||
        order->assignedRiderId != riderId || !isfinite(travelTime) ||
        travelTime < 0.0 || getDeliveryByOrderId(orderId) != nullptr ||
        !riderManager.searchRider(riderId, rider) || rider.status != "Busy")
        return false;

    Delivery delivery(orderId, riderId, travelTime);
    if (!storageManager.insertDelivery(delivery))
        return false;

    deliveries.push_back(delivery);
    return true;
}

bool FoodDeliverySystem::setRestaurantLocation(int restaurantId, int locationId)
{
    if (locationId < 0 || cityGraph.getVertexIndex(locationId) < 0)
        return false;
    const Restaurant *rest = searchManager.getRestaurantById(restaurantId);
    if (rest == nullptr)
        return false;
    Restaurant updatedRest = *rest;
    updatedRest.locationId = locationId;

    bool storageOk = false;
    if (storageManager.restaurantExists(restaurantId))
        storageOk = storageManager.updateRestaurant(updatedRest);
    else
        storageOk = storageManager.insertRestaurant(updatedRest);

    if (!storageOk)
        return false;

    return searchManager.setRestaurantLocation(restaurantId, locationId);
}

int FoodDeliverySystem::getRestaurantLocation(int restaurantId) const
{
    return searchManager.getRestaurantLocation(restaurantId);
}

bool FoodDeliverySystem::saveRestaurant(const Restaurant &restaurant)
{
    const Restaurant *existing = searchManager.getRestaurantById(restaurant.id);
    if (existing == nullptr)
    {
        searchManager.addRestaurant(restaurant.name);
        searchManager.setRestaurantLocation(restaurant.id, restaurant.locationId);
    }
    else
    {
        searchManager.setRestaurantLocation(restaurant.id, restaurant.locationId);
    }

    if (storageManager.restaurantExists(restaurant.id))
        return storageManager.updateRestaurant(restaurant);
    else
        return storageManager.insertRestaurant(restaurant);
}

bool FoodDeliverySystem::saveOrder(const Order &order)
{
    Order *existing = getOrderById(order.orderId);
    if (existing != nullptr)
    {
        *existing = order;
        return storageManager.updateOrder(order);
    }
    else
    {
        return addOrder(order) && storageManager.insertOrder(order);
    }
}

bool FoodDeliverySystem::saveDelivery(const Delivery &delivery)
{
    Delivery *existing = getDeliveryByOrderId(delivery.orderId);
    if (existing != nullptr)
    {
        *existing = delivery;
        return storageManager.updateDelivery(delivery);
    }
    else
    {
        deliveries.push_back(delivery);
        return storageManager.insertDelivery(delivery);
    }
}

bool FoodDeliverySystem::saveRider(const Rider &rider)
{
    Rider existing;
    if (riderManager.searchRider(rider.id, existing))
    {
        riderManager.updateRider(rider);
        if (storageManager.riderExists(rider.id))
            return storageManager.updateRider(rider);
        else
            return storageManager.insertRider(rider);
    }
    else
    {
        if (riderManager.insertRider(rider))
        {
            if (storageManager.riderExists(rider.id))
                return storageManager.updateRider(rider);
            else
                return storageManager.insertRider(rider);
        }
        return false;
    }
}

bool FoodDeliverySystem::addRestaurant(const string &name, int locationId)
{
    if (name.empty() || locationId < 0 || cityGraph.getVertexIndex(locationId) < 0)
        return false;

    int newId = static_cast<int>(searchManager.getRestaurantCount());
    searchManager.addRestaurant(name);
    searchManager.setRestaurantLocation(newId, locationId);

    Restaurant r(newId, name, locationId);
    return storageManager.insertRestaurant(r);
}

bool FoodDeliverySystem::addRider(const Rider &rider)
{
    if (!riderManager.insertRider(rider))
        return false;
    if (!storageManager.insertRider(rider))
    {
        riderManager.deleteRider(rider.id);
        return false;
    }
    return true;
}

bool FoodDeliverySystem::updateRider(const Rider &rider)
{
    Rider oldRider;
    if (!riderManager.searchRider(rider.id, oldRider))
        return false;

    if (!riderManager.updateRider(rider))
        return false;

    if (!storageManager.updateRider(rider))
    {
        riderManager.updateRider(oldRider);
        return false;
    }
    return true;
}

bool FoodDeliverySystem::updateRiderStatus(int riderId, const string &status)
{
    Rider rider;
    if (!riderManager.searchRider(riderId, rider))
        return false;
    rider.status = status;
    return updateRider(rider);
}

bool FoodDeliverySystem::updateRoadTraffic(int source, int destination, double travelTime)
{
    return cityGraph.updateTraffic(source, destination, travelTime);
}

bool FoodDeliverySystem::closeRoad(int source, int destination)
{
    return cityGraph.closeRoad(source, destination);
}

bool FoodDeliverySystem::reopenRoad(int source, int destination)
{
    return cityGraph.reopenRoad(source, destination);
}

bool FoodDeliverySystem::addMenuItem(const MenuItem &item)
{
    return storageManager.insertMenuItem(item);
}

vector<MenuItem> FoodDeliverySystem::getAllMenuItems()
{
    return storageManager.getAllMenuItems();
}

vector<Restaurant> FoodDeliverySystem::getAllRestaurants() const
{
    return searchManager.getAllRestaurants();
}

vector<Rider> FoodDeliverySystem::getAllRiders() const
{
    return riderManager.getAllRiders();
}

vector<Rider> FoodDeliverySystem::getAvailableRiders() const
{
    return riderManager.getAvailableRiders();
}

vector<string> FoodDeliverySystem::searchRestaurants(const string &query) const
{
    return searchManager.getSuggestion(query);
}

RouteResult FoodDeliverySystem::findShortestRoute(int sourceLocationId, int destLocationId) const
{
    return routeManager.shortestPath(sourceLocationId, destLocationId);
}
