#include "../../include/storage/StorageManager.h"

#include <cmath>
#include <filesystem>

using namespace std;

static const size_t MAX_SAFE_STRING_LENGTH = 10000;

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
    lock_guard<recursive_mutex> guard(storageMutex);
    dataFolder = folder;
    initialize();
}

bool StorageManager::initialize()
{
    namespace fs = filesystem;
    try
    {
        if (!fs::exists(dataFolder) && !fs::create_directories(dataFolder))
            return false;
    }
    catch (const fs::filesystem_error &)
    {
        return false;
    }
    return rebuildIndexes();
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
    size_t length = 0;
    if (!file.read(reinterpret_cast<char *>(&restaurant.id), sizeof(int)))
        return false;

    if (!file.read(reinterpret_cast<char *>(&restaurant.locationId), sizeof(int)))
        return false;
    if (!file.read(reinterpret_cast<char *>(&length), sizeof(size_t)) || length > MAX_SAFE_STRING_LENGTH)
        return false;
    restaurant.name.resize(length);
    if (length > 0)
        file.read(&restaurant.name[0], length);

    return file.good();
}

bool StorageManager::insertRestaurant(const Restaurant &restaurant)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    if (restaurant.id < 0 || restaurant.name.empty() ||
        restaurantIndex.contains(restaurant.id))
        return false;
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
    lock_guard<recursive_mutex> guard(storageMutex);
    ifstream file(dataFolder + "/restaurants.dat", ios::binary);
    if (!file)
        return false;
    Restaurant current;
    while (readRestaurant(file, current))
    {
        if (current.id == id)
        {
            result = current;
            return true;
        }
    }
    return false;
}

bool StorageManager::updateRestaurant(const Restaurant &restaurant)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<Restaurant> restaurants = getAllRestaurants();
    bool found = false;
    for (Restaurant &current : restaurants)
    {
        if (current.id == restaurant.id)
        {
            current = restaurant;
            found = true;
        }
    }
    if (!found)
        return false;
    ofstream file(dataFolder + "/restaurants.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const Restaurant &current : restaurants)
    {
        if (!writeRestaurant(file, current))
        {
            writeOk = false;
            break;
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    return writeOk;
}

bool StorageManager::deleteRestaurant(int id)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<Restaurant> restaurants = getAllRestaurants();
    const bool found = restaurantIndex.contains(id);
    if (!found)
        return false;
    ofstream file(dataFolder + "/restaurants.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const Restaurant &restaurant : restaurants)
    {
        if (restaurant.id != id)
        {
            if (!writeRestaurant(file, restaurant))
            {
                writeOk = false;
                break;
            }
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    if (writeOk)
        restaurantIndex.remove(id);
    return writeOk;
}

bool StorageManager::restaurantExists(int id) const
{
    return restaurantIndex.contains(id);
}

vector<Restaurant> StorageManager::getAllRestaurants()
{
    lock_guard<recursive_mutex> guard(storageMutex);
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
    size_t nameLength = 0;
    size_t categoryLength = 0;

    if (!file.read((char *)&item.id, sizeof(int)))
        return false;
    file.read((char *)&item.restaurantId, sizeof(int));
    file.read((char *)&item.price, sizeof(double));
    if (!file.read((char *)&nameLength, sizeof(size_t)) || nameLength > MAX_SAFE_STRING_LENGTH)
        return false;
    item.name.resize(nameLength);
    if (nameLength > 0)
        file.read(&item.name[0], nameLength);
    if (!file.read((char *)&categoryLength, sizeof(size_t)) || categoryLength > MAX_SAFE_STRING_LENGTH)
        return false;
    item.category.resize(categoryLength);
    if (categoryLength > 0)
        file.read(&item.category[0], categoryLength);

    return file.good();
}

bool StorageManager::insertMenuItem(const MenuItem &item)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    if (item.id < 0 || item.restaurantId < 0 || item.name.empty() ||
        item.category.empty() || item.price < 0.0 ||
        menuItemIndex.contains(item.id))
        return false;
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
    lock_guard<recursive_mutex> guard(storageMutex);
    ifstream file(dataFolder + "/menuitems.dat", ios::binary);
    if (!file)
        return false;
    MenuItem current;
    while (readMenuItem(file, current))
    {
        if (current.id == id)
        {
            item = current;
            return true;
        }
    }
    return false;
}

bool StorageManager::updateMenuItem(const MenuItem &item)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<MenuItem> items = getAllMenuItems();
    bool found = false;
    for (MenuItem &current : items)
    {
        if (current.id == item.id)
        {
            current = item;
            found = true;
        }
    }
    if (!found)
        return false;
    ofstream file(dataFolder + "/menuitems.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const MenuItem &current : items)
    {
        if (!writeMenuItem(file, current))
        {
            writeOk = false;
            break;
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    return writeOk;
}

bool StorageManager::deleteMenuItem(int id)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<MenuItem> items = getAllMenuItems();
    const bool found = menuItemIndex.contains(id);
    if (!found)
        return false;
    ofstream file(dataFolder + "/menuitems.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const MenuItem &item : items)
    {
        if (item.id != id)
        {
            if (!writeMenuItem(file, item))
            {
                writeOk = false;
                break;
            }
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    if (writeOk)
        menuItemIndex.remove(id);
    return writeOk;
}

bool StorageManager::menuItemExists(int id) const
{
    return menuItemIndex.contains(id);
}

vector<MenuItem> StorageManager::getAllMenuItems()
{
    lock_guard<recursive_mutex> guard(storageMutex);
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
    size_t nameLength = 0;

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
    if (!file.read((char *)&nameLength, sizeof(size_t)) || nameLength > MAX_SAFE_STRING_LENGTH)
        return false;

    order.restaurantName.resize(nameLength);
    if (nameLength > 0)
        file.read(&order.restaurantName[0], nameLength);

    return file.good();
}

bool StorageManager::insertOrder(const Order &order)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    if (order.orderId < 0 || order.restaurantId < 0 ||
        order.customerLocationId < 0 || order.priority < 0 ||
        order.prepTime < 0 || !isfinite(order.estimatedTravelTime) ||
        order.estimatedTravelTime < 0.0 ||
        orderIndex.contains(order.orderId))
        return false;
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
    lock_guard<recursive_mutex> guard(storageMutex);
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
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<Order> orders = getAllOrders();
    bool found = false;
    for (Order &current : orders)
    {
        if (current.orderId == order.orderId)
        {
            current = order;
            found = true;
        }
    }
    if (!found)
        return false;
    ofstream file(dataFolder + "/orders.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const Order &current : orders)
    {
        if (!writeOrder(file, current))
        {
            writeOk = false;
            break;
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    return writeOk;
}

bool StorageManager::deleteOrder(int id)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<Order> orders = getAllOrders();
    const bool found = orderIndex.contains(id);
    if (!found)
        return false;
    ofstream file(dataFolder + "/orders.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const Order &order : orders)
    {
        if (order.orderId != id)
        {
            if (!writeOrder(file, order))
            {
                writeOk = false;
                break;
            }
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    if (writeOk)
        orderIndex.remove(id);
    return writeOk;
}

bool StorageManager::orderExists(int id) const
{
    return orderIndex.contains(id);
}

vector<Order> StorageManager::getAllOrders()
{
    lock_guard<recursive_mutex> guard(storageMutex);
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
    lock_guard<recursive_mutex> guard(storageMutex);
    if (delivery.orderId < 0 || delivery.riderId < 0 ||
        !isfinite(delivery.travelTime) || delivery.travelTime < 0.0 ||
        deliveryIndex.contains(delivery.orderId))
        return false;
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
    lock_guard<recursive_mutex> guard(storageMutex);
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
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<Delivery> deliveries = getAllDeliveries();
    bool found = false;
    for (Delivery &current : deliveries)
    {
        if (current.orderId == delivery.orderId)
        {
            current = delivery;
            found = true;
        }
    }
    if (!found)
        return false;
    ofstream file(dataFolder + "/deliveries.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const Delivery &current : deliveries)
    {
        if (!writeDelivery(file, current))
        {
            writeOk = false;
            break;
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    return writeOk;
}

bool StorageManager::deleteDelivery(int orderId)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<Delivery> deliveries = getAllDeliveries();
    const bool found = deliveryIndex.contains(orderId);
    if (!found)
        return false;
    ofstream file(dataFolder + "/deliveries.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const Delivery &delivery : deliveries)
    {
        if (delivery.orderId != orderId)
        {
            if (!writeDelivery(file, delivery))
            {
                writeOk = false;
                break;
            }
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    if (writeOk)
        deliveryIndex.remove(orderId);
    return writeOk;
}

bool StorageManager::deliveryExists(int orderId) const
{
    return deliveryIndex.contains(orderId);
}

vector<Delivery> StorageManager::getAllDeliveries()
{
    lock_guard<recursive_mutex> guard(storageMutex);
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
    file.write((char *)&rider.locationId, sizeof(int));
    file.write((char *)&nameLength, sizeof(size_t));
    file.write(rider.name.c_str(), nameLength);
    file.write((char *)&statusLength, sizeof(size_t));
    file.write(rider.status.c_str(), statusLength);

    return file.good();
}

bool StorageManager::readRider(ifstream &file, Rider &rider)
{
    size_t nameLength = 0;
    size_t statusLength = 0;

    if (!file.read((char *)&rider.id, sizeof(int)))
        return false;

    file.read((char *)&rider.rating, sizeof(double));
    file.read((char *)&rider.assignedOrderId, sizeof(int));
    file.read((char *)&rider.locationId, sizeof(int));
    if (!file.read((char *)&nameLength, sizeof(size_t)) || nameLength > MAX_SAFE_STRING_LENGTH)
        return false;
    rider.name.resize(nameLength);
    if (nameLength > 0)
        file.read(&rider.name[0], nameLength);
    if (!file.read((char *)&statusLength, sizeof(size_t)) || statusLength > MAX_SAFE_STRING_LENGTH)
        return false;
    rider.status.resize(statusLength);
    if (statusLength > 0)
        file.read(&rider.status[0], statusLength);

    return file.good();
}

bool StorageManager::insertRider(const Rider &rider)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    if (rider.id < 0 || rider.name.empty() || rider.rating < 0.0 ||
        rider.rating > 5.0 || riderIndex.contains(rider.id))
        return false;
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
    lock_guard<recursive_mutex> guard(storageMutex);
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
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<Rider> riders = getAllRiders();
    bool found = false;
    for (Rider &current : riders)
    {
        if (current.id == rider.id)
        {
            current = rider;
            found = true;
        }
    }
    if (!found)
        return false;
    ofstream file(dataFolder + "/riders.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const Rider &current : riders)
    {
        if (!writeRider(file, current))
        {
            writeOk = false;
            break;
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    return writeOk;
}

bool StorageManager::deleteRider(int id)
{
    lock_guard<recursive_mutex> guard(storageMutex);
    vector<Rider> riders = getAllRiders();
    const bool found = riderIndex.contains(id);
    if (!found)
        return false;
    ofstream file(dataFolder + "/riders.dat", ios::binary | ios::trunc);
    if (!file)
        return false;
    bool writeOk = true;
    for (const Rider &rider : riders)
    {
        if (rider.id != id)
        {
            if (!writeRider(file, rider))
            {
                writeOk = false;
                break;
            }
        }
    }
    file.flush();
    if (!file.good())
        writeOk = false;
    if (writeOk)
        riderIndex.remove(id);
    return writeOk;
}

bool StorageManager::riderExists(int id) const
{
    return riderIndex.contains(id);
}

vector<Rider> StorageManager::getAllRiders()
{
    lock_guard<recursive_mutex> guard(storageMutex);
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