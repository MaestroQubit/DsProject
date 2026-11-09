#pragma once

#include "Trie.h"
#include <cstddef>
#include <string>
#include <vector>

class SearchManager
{
private:
    Trie trie;
    std::vector<std::string> restaurantNames;

public:
    SearchManager();

    void addRestaurant(const std::string& name);
    bool loadRestaurant(const std::string& filename);
    bool searchRestaurant(const std::string& name) const;
    bool hasPrefix(const std::string& prefix) const;
    std::vector<std::string> getSuggestion(const std::string& prefix, size_t limit = 10) const;
    std::size_t getRestaurantCount() const;
    std::vector<std::string> getAllRestaurants() const;
};