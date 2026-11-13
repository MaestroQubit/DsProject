#pragma once

#include "../models/Restaurant.h"
#include "../models/MenuItem.h"
#include "../models/Order.h"
#include "../models/Delivery.h"
#include "../models/Rider.h"
#include "BPlusTree.h"

#include <vector>
#include <string>
#include <fstream>
#include <mutex>

class StorageManager
{
private:
    std::string dataFolder;

    BPlusTree restaurantIndex;
    BPlusTree menuItemIndex;
    BPlusTree orderIndex;
    BPlusTree deliveryIndex;
    BPlusTree riderIndex;

    mutable std::recursive_mutex storageMutex;

    bool writeRestaurant(std::ofstream &file, const Restaurant &restaurant);
    bool readRestaurant(std::ifstream &file, Restaurant &restaurant);

    bool writeMenuItem(std::ofstream &file, const MenuItem &item);
    bool readMenuItem(std::ifstream &file, MenuItem &item);

    bool writeOrder(std::ofstream &file, const Order &order);
    bool readOrder(std::ifstream &file, Order &order);

    bool writeDelivery(std::ofstream &file, const Delivery &delivery);
    bool readDelivery(std::ifstream &file, Delivery &delivery);

    bool writeRider(std::ofstream &file, const Rider &rider);
    bool readRider(std::ifstream &file, Rider &rider);

public:
    explicit StorageManager(const std::string &folder = "data");
    StorageManager(const StorageManager &) = delete;
    StorageManager &operator=(const StorageManager &) = delete;

    bool initialize();
    void clearIndexes();

    std::string getDataFolder() const;

    void setDataFolder(const std::string &folder);
    bool rebuildIndexes();
    bool insertRestaurant(const Restaurant &restaurant);
    bool getRestaurant(int id, Restaurant &restaurant);
    bool updateRestaurant(const Restaurant &restaurant);
    bool deleteRestaurant(int id);
    bool restaurantExists(int id) const;

    std::vector<Restaurant> getAllRestaurants();

    bool insertMenuItem(const MenuItem &item);
    bool getMenuItem(int id, MenuItem &item);
    bool updateMenuItem(const MenuItem &item);
    bool deleteMenuItem(int id);
    bool menuItemExists(int id) const;

    std::vector<MenuItem> getAllMenuItems();

    bool insertOrder(const Order &order);
    bool getOrder(int id, Order &order);
    bool updateOrder(const Order &order);
    bool deleteOrder(int id);
    bool orderExists(int id) const;

    std::vector<Order> getAllOrders();

    bool insertDelivery(const Delivery &delivery);
    bool getDelivery(int orderId, Delivery &delivery);
    bool updateDelivery(const Delivery &delivery);
    bool deleteDelivery(int orderId);
    bool deliveryExists(int orderId) const;

    std::vector<Delivery> getAllDeliveries();

    bool insertRider(const Rider &rider);
    bool getRider(int id, Rider &rider);
    bool updateRider(const Rider &rider);
    bool deleteRider(int id);
    bool riderExists(int id) const;

    std::vector<Rider> getAllRiders();

    const BPlusTree &getRestaurantIndex() const;
    const BPlusTree &getMenuItemIndex() const;
    const BPlusTree &getOrderIndex() const;
    const BPlusTree &getDeliveryIndex() const;
    const BPlusTree &getRiderIndex() const;
};