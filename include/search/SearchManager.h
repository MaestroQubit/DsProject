#pragma once

#include "Trie.h";
#include <string>
#include <vector>
using namespace std;

class SearchManager
{
private:
    Trie trie;
    vector<string> restaurantNames;

public:
    SearchManager();

    void addRestaurant(string name);
    void loadRestaurant(string filename);
    bool searchRestaurant(string name);
    bool hasPrefix(string prefix);
    vector<string> getSuggestion(string prefix, int limit = 10);
    int getRestaurantCount();
    vector<string> getAllRestaurants();
};