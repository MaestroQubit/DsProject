/*
====================================================
TRIE TEST FILE
====================================================

MODULE:
Trie + SearchManager

PURPOSE:
Tests all Trie operations:

1. insert()
2. search()
3. startWith()
4. autoComplete()

----------------------------------------------------
HOW TO COMPILE
----------------------------------------------------

From project root:

g++ tests/testTrie.cpp \
    src/search/Trie.cpp \
    src/search/SearchManager.cpp \
    -I include \
    -o testTrie

----------------------------------------------------
HOW TO RUN
----------------------------------------------------

Windows:

testTrie.exe

OR

./testTrie.exe

----------------------------------------------------
EXPECTED OUTPUT
----------------------------------------------------

Search Tests
1
1
0

Prefix Tests
1
1
0

Autocomplete Tests

pizzahut
pizzapoint

----------------------------------------------------
FUNCTION DESCRIPTIONS
----------------------------------------------------

addRestaurant(name)

    Adds a restaurant to the Trie.

    Example:

    addRestaurant("Dominos");

----------------------------------------------------

searchRestaurant(name)

    Searches for an exact restaurant name.

    Returns:
        true  -> found
        false -> not found

    Example:

    searchRestaurant("Dominos")

----------------------------------------------------

hasPrefix(prefix)

    Checks if any restaurant begins
    with the given prefix.

    Returns:
        true  -> prefix exists
        false -> prefix not found

    Example:

    hasPrefix("Pi")

----------------------------------------------------

getSuggestions(prefix)

    Returns restaurant suggestions
    beginning with the prefix.

    Example:

    getSuggestions("Pi")

    Output:

    Pizza Hut
    Pizza Point

----------------------------------------------------

NOTE

The Trie internally stores names
in normalized form:

    Pizza Hut
        ↓
    pizzahut

Searches are case-insensitive.

====================================================
*/

#include <iostream>
#include "../include/search/Trie.h"

using namespace std;

int main()
{
    Trie t;

    t.insert("Dominos");
    t.insert("Pizza Hut");
    t.insert("Pizza Point");
    t.insert("Burger King");
    t.insert("KFC");
    t.insert("Subway");

    cout << "Search Tests\n";
    cout << t.search("Dominos") << endl;
    cout << t.search("Pizza Hut") << endl;
    cout << t.search("McDonalds") << endl;

    cout << "\nPrefix Tests\n";
    cout << t.startWith("Pi") << endl;
    cout << t.startWith("Bur") << endl;
    cout << t.startWith("XYZ") << endl;

    cout << "\nAutocomplete Tests\n";

    vector<string> res =
        t.autoComplete("Pi");

    for(string s : res)
    {
        cout << s << endl;
    }

    return 0;
}