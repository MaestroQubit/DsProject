#include "../../include/search/SearchManager.h"
#include <sstream>
#include <fstream>
#include <stdexcept>
using namespace std;

bool containsAlphabeticCharacter(const string &value)
{
    for (char character : value)
    {
        const unsigned char value = static_cast<unsigned char>(character);
        if ((value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z'))
            return true;
    }
    return false;
}

SearchManager::SearchManager() = default;

void SearchManager::addRestaurant(const string &name)
{
    if (name.empty() || !containsAlphabeticCharacter(name) || searchRestaurant(name))
        return;

    trie.insert(name);
    Restaurant restaurant(static_cast<int>(restaurants.size()), name, -1);
    restaurants.push_back(restaurant);
}

bool SearchManager::loadRestaurant(const string &filename)
{
    ifstream fin(filename);
    if (!fin)
        return false;

    vector<Restaurant> loaded;
    string line;
    if (!getline(fin, line))
        return false;

    while (getline(fin, line))
    {
        stringstream ss(line);
        string idToken;
        string name;
        string locationToken;
        if (!getline(ss, idToken, ',') || !getline(ss, name, ',') ||
            !getline(ss, locationToken))
            return false;

        int id = -1;
        int locationId = -1;
        try
        {
            id = stoi(idToken);
            locationId = stoi(locationToken);
        }
        catch (const invalid_argument &)
        {
            return false;
        }
        catch (const out_of_range &)
        {
            return false;
        }

        if (id < 0 || name.empty() || !containsAlphabeticCharacter(name) ||
            getRestaurantById(id) != nullptr)
            return false;

        for (const Restaurant &restaurant : loaded)
        {
            if (restaurant.id == id)
                return false;
        }
        loaded.push_back(Restaurant(id, name, locationId));
    }

    for (const Restaurant &restaurant : loaded)
    {
        trie.insert(restaurant.name);
        restaurants.push_back(restaurant);
    }

    return true;
}

bool SearchManager::searchRestaurant(const string &name) const
{
    return trie.search(name);
}

bool SearchManager::hasPrefix(const string &prefix) const
{
    return trie.startWith(prefix);
}

vector<string> SearchManager::getSuggestion(const string &prefix, size_t limit) const
{
    return trie.autoComplete(prefix, limit);
}

size_t SearchManager::getRestaurantCount() const
{
    return restaurants.size();
}

std::vector<Restaurant> SearchManager::getAllRestaurants() const
{
    return restaurants;
}

bool SearchManager::setRestaurantLocation(int restaurantId, int locationId)
{
    for (Restaurant &restaurant : restaurants)
    {
        if (restaurant.id == restaurantId)
        {
            restaurant.locationId = locationId;
            return true;
        }
    }
    return false;
}

int SearchManager::getRestaurantLocation(int restaurantId) const
{
    const Restaurant *restaurant = getRestaurantById(restaurantId);
    return restaurant == nullptr ? -1 : restaurant->locationId;
}

const Restaurant *SearchManager::getRestaurantById(int restaurantId) const
{
    for (const Restaurant &restaurant : restaurants)
    {
        if (restaurant.id == restaurantId)
            return &restaurant;
    }
    return nullptr;
}