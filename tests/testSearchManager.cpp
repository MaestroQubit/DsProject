/*
====================================================
SEARCH MANAGER TEST FILE
====================================================

MODULE:
SearchManager

DEPENDENCIES:
1. SearchManager
2. Trie

PURPOSE:
Tests the complete restaurant search system.

The following features are tested:

1. loadRestaurants()
2. addRestaurant()
3. searchRestaurant()
4. hasPrefix()
5. getSuggestions()
6. getRestaurantCount()

====================================================
PROJECT STRUCTURE REQUIRED
====================================================

FoodDeliveryManagementSystem/

│
├── include/
│   └── search/
│       ├── Trie.h
│       └── SearchManager.h
│
├── src/
│   └── search/
│       ├── Trie.cpp
│       └── SearchManager.cpp
│
├── data/
│   └── restaurants.txt
│
└── tests/
    └── testSearchManager.cpp

====================================================
SAMPLE restaurants.txt
====================================================

Dominos
Pizza Hut
Pizza Point
Burger King
KFC
Subway
McDonalds

====================================================
HOW TO COMPILE
====================================================

Run from project root folder:

g++ tests/testSearchManager.cpp ^
    src/search/Trie.cpp ^
    src/search/SearchManager.cpp ^
    -I include ^
    -o testSearchManager

OR (single line)

g++ tests/testSearchManager.cpp src/search/Trie.cpp src/search/SearchManager.cpp -I include -o testSearchManager

====================================================
HOW TO RUN
====================================================

Windows:

testSearchManager.exe

OR

./testSearchManager.exe

====================================================
FUNCTION DESCRIPTIONS
====================================================

loadRestaurants(filename)

Purpose:
Loads all restaurant names
from a text file.

Input:
"data/restaurants.txt"

Output:
Restaurants inserted into Trie.

----------------------------------------------------

addRestaurant(name)

Purpose:
Insert a new restaurant.

Example:

addRestaurant("Haldirams")

----------------------------------------------------

searchRestaurant(name)

Purpose:
Performs exact restaurant search.

Returns:

true  -> restaurant exists

false -> restaurant not found

Example:

searchRestaurant("Dominos")

----------------------------------------------------

hasPrefix(prefix)

Purpose:
Checks whether at least one
restaurant begins with the prefix.

Returns:

true  -> prefix exists

false -> prefix does not exist

Example:

hasPrefix("Pi")

----------------------------------------------------

getSuggestions(prefix)

Purpose:
Returns autocomplete suggestions.

Input:

"Pi"

Output:

Pizza Hut
Pizza Point

----------------------------------------------------

getRestaurantCount()

Purpose:
Returns total number of restaurants
currently loaded.

Example:

7

====================================================
EXPECTED OUTPUT
====================================================

Total Restaurants: 7

Search Tests

Dominos : 1
XYZ : 0

Prefix Tests

Pi : 1
Mc : 1
ZZ : 0

Autocomplete Tests

pizzahut
pizzapoint

====================================================
NOTES
====================================================

1. Searches are case-insensitive.

Example:

DOMINOS
Dominos
dominos

all produce same result.

----------------------------------------------------

2. Trie internally normalizes names.

Example:

Pizza Hut

stored as

pizzahut

----------------------------------------------------

3. Duplicate restaurants are ignored.

Example:

addRestaurant("Dominos");
addRestaurant("Dominos");

Only one copy is stored.

====================================================
*/

#include "../include/search/SearchManager.h"

using namespace std;

int main()
{
    SearchManager sm;

    sm.loadRestaurant(
        "../data/restaurants.txt"
    );

    cout << "Total Restaurants : "
         << sm.getRestaurantCount()
         << endl;

    cout << endl;

    cout << "Dominos : "
         << sm.searchRestaurant("Dominos")
         << endl;

    cout << "XYZ : "
         << sm.searchRestaurant("XYZ")
         << endl;

    cout << endl;

    cout << "Has Prefix Pi : "
         << sm.hasPrefix("Pi")
         << endl;

    cout << endl;

    vector<string> v =
        sm.getSuggestion("Pi");

    cout << "Suggestions:\n";

    for(string s : v)
        cout << s << endl;

    return 0;
}