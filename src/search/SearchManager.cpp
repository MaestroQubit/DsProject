#include "../../include/search/SearchManager.h"
#include <cctype>
#include <fstream>
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

    Trie candidateTrie;
    candidateTrie.insert(name);
    if (!candidateTrie.search(name))
        return;

    trie.insert(name);
    restaurantNames.push_back(name);
}
bool SearchManager::loadRestaurant(const string &filename)
{
    ifstream fin(filename);
    if (!fin)
        return false;
    string name;
    while (getline(fin, name))
    {
        if (!name.empty())
            addRestaurant(name);
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
    return restaurantNames.size();
}
vector<string> SearchManager::getAllRestaurants() const
{
    return restaurantNames;
}