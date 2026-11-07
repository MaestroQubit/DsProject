#include "../../include/search/SearchManager.h"
#include <fstream>

SearchManager::SearchManager() {}

void SearchManager::addRestaurant(string name)
{
    if(searchRestaurant(name))
        return;

    trie.insert(name);
    restaurantNames.push_back(name);
}

void SearchManager::loadRestaurant(string filename)
{
    ifstream fin(filename);

    string name;

    while(getline(fin, name))
    {
        if(!name.empty())
            addRestaurant(name);
    }

    fin.close();
}

bool SearchManager::searchRestaurant(string name)
{
    return trie.search(name);
}

bool SearchManager::hasPrefix(string prefix)
{
    return trie.startWith(prefix);
}

vector<string> SearchManager::getSuggestion(string prefix, int limit)
{
    return trie.autoComplete(prefix, limit);
}

int SearchManager::getRestaurantCount()
{
    return restaurantNames.size();
}

vector<string> SearchManager::getAllRestaurants()
{
    return restaurantNames;
}