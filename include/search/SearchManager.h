#pragma once

#include "../models/Restaurant.h"
#include "Trie.h"
#include <cstddef>
#include <string>
#include <vector>

class SearchManager
{
private:
    Trie trie;
    std::vector<Restaurant> restaurants;

public:
    SearchManager();
    SearchManager(const SearchManager &) = delete;
    SearchManager &operator=(const SearchManager &) = delete;

    void addRestaurant(const std::string &name);
    void clear();
    bool loadRestaurant(const std::string &filename);
    bool searchRestaurant(const std::string &name) const;
    bool hasPrefix(const std::string &prefix) const;
    std::vector<std::string> getSuggestion(const std::string &prefix, size_t limit = 10) const;
    std::size_t getRestaurantCount() const;
    std::vector<Restaurant> getAllRestaurants() const;
    bool setRestaurantLocation(int restaurantId,int locationId);
    int getRestaurantLocation(int restaurantId) const;
    const Restaurant *getRestaurantById(int restaurantId) const;
    const Restaurant *getRestaurantByName(const std::string &name) const;
};