#include "../../include/storage/StorageManager.h"

#include <filesystem>

using namespace std;

StorageManager::StorageManager(const string &folder)
{
    dataFolder = folder;
}

string StorageManager::getDataFolder() const
{
    return dataFolder;
}

void StorageManager::setDataFolder(const string &folder)
{
    dataFolder = folder;
}

bool StorageManager::initialize()
{
    namespace fs = filesystem;
    if (!fs::exists(dataFolder))
        fs::create_directories(dataFolder);
    rebuildIndexes();
    return true;
}

void StorageManager::clearIndexes()
{
    restaurantIndex.clear();
    menuItemIndex.clear();
    orderIndex.clear();
    deliveryIndex.clear();
    riderIndex.clear();
}

bool StorageManager::rebuildIndexes()
{
    clearIndexes();

    vector<Restaurant> restaurants = getAllRestaurants();
    for (const Restaurant &r : restaurants)
        restaurantIndex.insert(r.id, r.id);

    vector<MenuItem> menuItems = getAllMenuItems();
    for (const MenuItem &item : menuItems)
        menuItemIndex.insert(item.id, item.id);

    vector<Order> orders = getAllOrders();
    for (const Order &order : orders)
        orderIndex.insert(order.orderId, order.orderId);

    vector<Delivery> deliveries = getAllDeliveries();
    for (const Delivery &delivery : deliveries)
        deliveryIndex.insert(delivery.orderId, delivery.orderId);

    vector<Rider> riders = getAllRiders();
    for (const Rider &rider : riders)
        riderIndex.insert(rider.id, rider.id);

    return true;
}

bool StorageManager::writeRestaurant(ofstream &file, const Restaurant &restaurant)
{
    size_t length = restaurant.name.size();

    file.write(reinterpret_cast<const char *>(&restaurant.id), sizeof(int));
    file.write(reinterpret_cast<const char *>(&restaurant.locationId), sizeof(int));
    file.write(reinterpret_cast<const char *>(&length), sizeof(size_t));
    file.write(restaurant.name.c_str(), length);

    return file.good();
}

bool StorageManager::readRestaurant(ifstream &file, Restaurant &restaurant)
{
    size_t length;
    if (!file.read(reinterpret_cast<char *>(&restaurant.id), sizeof(int)))
        return false;

    file.read(reinterpret_cast<char *>(&restaurant.locationId), sizeof(int));
    file.read(reinterpret_cast<char *>(&length), sizeof(size_t));
    restaurant.name.resize(length);
    file.read(&restaurant.name[0], length);

    return file.good();
}

bool StorageManager::insertRestaurant(const Restaurant &restaurant)
{
    lock_guard<mutex> guard(storageMutex);
    ofstream file(dataFolder + "/restaurants.dat", ios::binary | ios::app);
    if (!file)
        return false;
    bool success = writeRestaurant(file, restaurant);
    if (success)
        restaurantIndex.insert(restaurant.id, restaurant.id);
    return success;
}

bool StorageManager::getRestaurant(int id, Restaurant &result)
{
    lock_guard<mutex> guard(storageMutex);
    ifstream file(dataFolder + "/restaurants.dat", ios::binary);
    if (!file)
        return false;
    Restaurant current;
    while (readRestaurant(file, current))
    {
        if (current.id == id)
            result = current;
        return true;
    }
    return false;
}

bool StorageManager::updateRestaurant(const Restaurant &restaurant)
{
    vector<Restaurant> restaurants = getAllRestaurants();
    for (Restaurant &current : restaurants)
    {
        if (current.id == restaurant.id)
            current = restaurant;
    }
    ofstream file(dataFolder + "/restaurants.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const Restaurant &current : restaurants)
        writeRestaurant(file, current);
    return true;
}

bool StorageManager::deleteRestaurant(int id)
{
    vector<Restaurant> restaurants = getAllRestaurants();
    ofstream file(dataFolder + "/restaurants.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const Restaurant &restaurant : restaurants)
    {
        if (restaurant.id != id)
            writeRestaurant(file, restaurant);
    }
    restaurantIndex.remove(id);
    return true;
}

bool StorageManager::restaurantExists(int id) const
{
    return restaurantIndex.contains(id);
}

vector<Restaurant> StorageManager::getAllRestaurants()
{
    lock_guard<mutex> guard(storageMutex);
    vector<Restaurant> result;
    ifstream file(dataFolder + "/restaurants.dat", ios::binary);
    if (!file)
        return result;
    Restaurant restaurant;
    while (readRestaurant(file, restaurant))
        result.push_back(restaurant);
    return result;
}

bool StorageManager::writeMenuItem(ofstream &file, const MenuItem &item)
{
    size_t nameLength = item.name.size();
    size_t categoryLength = item.category.size();

    file.write((char *)&item.id, sizeof(int));
    file.write((char *)&item.restaurantId, sizeof(int));
    file.write((char *)&item.price, sizeof(double));
    file.write((char *)&nameLength, sizeof(size_t));
    file.write(item.name.c_str(), nameLength);
    file.write((char *)&categoryLength, sizeof(size_t));
    file.write(item.category.c_str(), categoryLength);

    return file.good();
}

bool StorageManager::readMenuItem(ifstream &file, MenuItem &item)
{
    size_t nameLength;
    size_t categoryLength;

    if (!file.read((char *)&item.id, sizeof(int)))
        return false;
    file.read((char *)&item.restaurantId, sizeof(int));
    file.read((char *)&item.price, sizeof(double));
    file.read((char *)&nameLength, sizeof(size_t));
    item.name.resize(nameLength);
    file.read(&item.name[0], nameLength);
    file.read((char *)&categoryLength, sizeof(size_t));
    item.category.resize(categoryLength);
    file.read(&item.category[0], categoryLength);

    return file.good();
}

bool StorageManager::insertMenuItem(const MenuItem &item)
{
    lock_guard<mutex> guard(storageMutex);
    ofstream file(dataFolder + "/menuitems.dat", ios::binary | ios::app);
    if (!file)
        return false;
    bool success = writeMenuItem(file, item);
    if (success)
        menuItemIndex.insert(item.id, item.id);
    return success;
}

bool StorageManager::getMenuItem(int id, MenuItem &item)
{
    lock_guard<mutex> guard(storageMutex);
    ifstream file(dataFolder + "/menuitems.dat", ios::binary);
    if (!file)
        return false;
    MenuItem current;
    while (readMenuItem(file, current))
    {
        if (current.id == id)
            item = current;
        return true;
    }
    return false;
}

bool StorageManager::updateMenuItem(const MenuItem &item)
{
    vector<MenuItem> items = getAllMenuItems();
    for (MenuItem &current : items)
    {
        if (current.id == item.id)
            current = item;
    }
    ofstream file(dataFolder + "/menuitems.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const MenuItem &current : items)
        writeMenuItem(file, current);
    return true;
}

bool StorageManager::deleteMenuItem(int id)
{
    vector<MenuItem> items = getAllMenuItems();
    ofstream file(dataFolder + "/menuitems.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const MenuItem &item : items)
    {
        if (item.id != id)
            writeMenuItem(file, item);
    }
    menuItemIndex.remove(id);
    return true;
}

bool StorageManager::menuItemExists(int id) const
{
    return menuItemIndex.contains(id);
}

vector<MenuItem> StorageManager::getAllMenuItems()
{
    lock_guard<mutex> guard(storageMutex);
    vector<MenuItem> result;
    ifstream file(dataFolder + "/menuitems.dat", ios::binary);
    if (!file)
        return result;
    MenuItem item;
    while (readMenuItem(file, item))
        result.push_back(item);
    return result;
}

bool StorageManager::writeOrder(ofstream &file, const Order &order)
{
    size_t nameLength = order.restaurantName.size();

    file.write((char *)&order.orderId, sizeof(int));
    file.write((char *)&order.restaurantId, sizeof(int));
    file.write((char *)&order.customerLocationId, sizeof(int));
    file.write((char *)&order.assignedRiderId, sizeof(int));
    file.write((char *)&order.priority, sizeof(int));
    file.write((char *)&order.prepTime, sizeof(int));
    file.write((char *)&order.estimatedTravelTime, sizeof(double));
    file.write((char *)&order.prepared, sizeof(bool));
    file.write((char *)&order.riderAssigned, sizeof(bool));
    file.write((char *)&order.delivered, sizeof(bool));
    file.write((char *)&nameLength, sizeof(size_t));
    file.write(order.restaurantName.c_str(), nameLength);

    return file.good();
}

bool StorageManager::readOrder(ifstream &file, Order &order)
{
    size_t nameLength;

    if (!file.read((char *)&order.orderId, sizeof(int)))
        return false;

    file.read((char *)&order.restaurantId, sizeof(int));
    file.read((char *)&order.customerLocationId, sizeof(int));
    file.read((char *)&order.assignedRiderId, sizeof(int));
    file.read((char *)&order.priority, sizeof(int));
    file.read((char *)&order.prepTime, sizeof(int));
    file.read((char *)&order.estimatedTravelTime, sizeof(double));
    file.read((char *)&order.prepared, sizeof(bool));
    file.read((char *)&order.riderAssigned, sizeof(bool));
    file.read((char *)&order.delivered, sizeof(bool));
    file.read((char *)&nameLength, sizeof(size_t));

    order.restaurantName.resize(nameLength);

    file.read(&order.restaurantName[0], nameLength);

    return file.good();
}

bool StorageManager::insertOrder(const Order &order)
{
    lock_guard<mutex> guard(storageMutex);
    ofstream file(dataFolder + "/orders.dat", ios::binary | ios::app);
    if (!file)
        return false;
    bool success = writeOrder(file, order);
    if (success)
        orderIndex.insert(order.orderId, order.orderId);
    return success;
}

bool StorageManager::getOrder(int id, Order &order)
{
    lock_guard<mutex> guard(storageMutex);
    ifstream file(dataFolder + "/orders.dat", ios::binary);
    if (!file)
        return false;
    Order current;
    while (readOrder(file, current))
    {
        if (current.orderId == id)
        {
            order = current;
            return true;
        }
    }
    return false;
}

bool StorageManager::updateOrder(const Order &order)
{
    vector<Order> orders = getAllOrders();
    for (Order &current : orders)
    {
        if (current.orderId == order.orderId)
            current = order;
    }
    ofstream file(dataFolder + "/orders.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const Order &current : orders)
        writeOrder(file, current);
    return true;
}

bool StorageManager::deleteOrder(int id)
{
    vector<Order> orders = getAllOrders();
    ofstream file(dataFolder + "/orders.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const Order &order : orders)
    {
        if (order.orderId != id)
            writeOrder(file, order);
    }
    orderIndex.remove(id);
    return true;
}

bool StorageManager::orderExists(int id) const
{
    return orderIndex.contains(id);
}

vector<Order> StorageManager::getAllOrders()
{
    lock_guard<mutex> guard(storageMutex);
    vector<Order> result;
    ifstream file(dataFolder + "/orders.dat", ios::binary);
    if (!file)
        return result;
    Order order;
    while (readOrder(file, order))
        result.push_back(order);
    return result;
}

bool StorageManager::writeDelivery(ofstream &file, const Delivery &delivery)
{
    file.write((char *)&delivery.orderId, sizeof(int));
    file.write((char *)&delivery.riderId, sizeof(int));
    file.write((char *)&delivery.travelTime, sizeof(double));
    file.write((char *)&delivery.completed, sizeof(bool));

    return file.good();
}

bool StorageManager::readDelivery(ifstream &file, Delivery &delivery)
{
    if (!file.read((char *)&delivery.orderId, sizeof(int)))
        return false;

    file.read((char *)&delivery.riderId, sizeof(int));
    file.read((char *)&delivery.travelTime, sizeof(double));
    file.read((char *)&delivery.completed, sizeof(bool));

    return file.good();
}

bool StorageManager::insertDelivery(const Delivery &delivery)
{
    lock_guard<mutex> guard(storageMutex);
    ofstream file(dataFolder + "/deliveries.dat", ios::binary | ios::app);
    if (!file)
        return false;
    bool success = writeDelivery(file, delivery);
    if (success)
        deliveryIndex.insert(delivery.orderId, delivery.orderId);
    return success;
}

bool StorageManager::getDelivery(int orderId, Delivery &delivery)
{
    lock_guard<mutex> guard(storageMutex);
    ifstream file(dataFolder + "/deliveries.dat", ios::binary);
    if (!file)
        return false;
    Delivery current;
    while (readDelivery(file, current))
    {
        if (current.orderId == orderId)
        {
            delivery = current;
            return true;
        }
    }
    return false;
}

bool StorageManager::updateDelivery(const Delivery &delivery)
{
    vector<Delivery> deliveries = getAllDeliveries();
    for (Delivery &current : deliveries)
    {
        if (current.orderId == delivery.orderId)
            current = delivery;
    }
    ofstream file(dataFolder + "/deliveries.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const Delivery &current : deliveries)
        writeDelivery(file, current);
    return true;
}

bool StorageManager::deleteDelivery(int orderId)
{
    vector<Delivery> deliveries = getAllDeliveries();
    ofstream file(dataFolder + "/deliveries.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const Delivery &delivery : deliveries)
    {
        if (delivery.orderId != orderId)
            writeDelivery(file, delivery);
    }
    deliveryIndex.remove(orderId);
    return true;
}

bool StorageManager::deliveryExists(int orderId) const
{
    return deliveryIndex.contains(orderId);
}

vector<Delivery> StorageManager::getAllDeliveries()
{
    lock_guard<mutex> guard(storageMutex);
    vector<Delivery> result;
    ifstream file(dataFolder + "/deliveries.dat", ios::binary);
    if (!file)
        return result;
    Delivery delivery;
    while (readDelivery(file, delivery))
        result.push_back(delivery);
    return result;
}

bool StorageManager::writeRider(ofstream &file, const Rider &rider)
{
    size_t nameLength = rider.name.size();
    size_t statusLength = rider.status.size();

    file.write((char *)&rider.id, sizeof(int));
    file.write((char *)&rider.rating, sizeof(double));
    file.write((char *)&rider.assignedOrderId, sizeof(int));
    file.write((char *)&nameLength, sizeof(size_t));
    file.write(rider.name.c_str(), nameLength);
    file.write((char *)&statusLength, sizeof(size_t));
    file.write(rider.status.c_str(), statusLength);

    return file.good();
}

bool StorageManager::readRider(ifstream &file, Rider &rider)
{
    size_t nameLength;
    size_t statusLength;

    if (!file.read((char *)&rider.id, sizeof(int)))
        return false;

    file.read((char *)&rider.rating, sizeof(double));
    file.read((char *)&rider.assignedOrderId, sizeof(int));
    file.read((char *)&nameLength, sizeof(size_t));
    rider.name.resize(nameLength);
    file.read(&rider.name[0], nameLength);
    file.read((char *)&statusLength, sizeof(size_t));
    rider.status.resize(statusLength);
    file.read(&rider.status[0], statusLength);

    return file.good();
}

bool StorageManager::insertRider(const Rider &rider)
{
    lock_guard<mutex> guard(storageMutex);
    ofstream file(dataFolder + "/riders.dat", ios::binary | ios::app);
    if (!file)
        return false;
    bool success = writeRider(file, rider);
    if (success)
        riderIndex.insert(rider.id, rider.id);
    return success;
}

bool StorageManager::getRider(int id, Rider &rider)
{
    lock_guard<mutex> guard(storageMutex);
    ifstream file(dataFolder + "/riders.dat", ios::binary);
    if (!file)
        return false;
    Rider current;
    while (readRider(file, current))
    {
        if (current.id == id)
        {
            rider = current;
            return true;
        }
    }
    return false;
}

bool StorageManager::updateRider(const Rider &rider)
{
    vector<Rider> riders = getAllRiders();
    for (Rider &current : riders)
    {
        if (current.id == rider.id)
            current = rider;
    }
    ofstream file(dataFolder + "/riders.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const Rider &current : riders)
        writeRider(file, current);
    return true;
}

bool StorageManager::deleteRider(int id)
{
    vector<Rider> riders = getAllRiders();
    ofstream file(dataFolder + "/riders.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    for (const Rider &rider : riders)
    {
        if (rider.id != id)
            writeRider(file, rider);
    }
    riderIndex.remove(id);
    return true;
}

bool StorageManager::riderExists(int id) const
{
    return riderIndex.contains(id);
}

vector<Rider> StorageManager::getAllRiders()
{
    lock_guard<mutex> guard(storageMutex);
    vector<Rider> result;
    ifstream file(dataFolder + "/riders.dat", ios::binary);
    if (!file)
        return result;
    Rider rider;
    while (readRider(file, rider))
        result.push_back(rider);
    return result;
}

const BPlusTree &StorageManager::getRestaurantIndex() const
{
    return restaurantIndex;
}

const BPlusTree &StorageManager::getMenuItemIndex() const
{
    return menuItemIndex;
}

const BPlusTree &StorageManager::getOrderIndex() const
{
    return orderIndex;
}

const BPlusTree &StorageManager::getDeliveryIndex() const
{
    return deliveryIndex;
}

const BPlusTree &StorageManager::getRiderIndex() const
{
    return riderIndex;
}