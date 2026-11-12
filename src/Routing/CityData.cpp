#include "../include/CityData.h"
#include <fstream>

using namespace std;

static LocationInfo locations[] = {
    {0, "Dominos", "Restaurant"},
    {1, "Pizza Hut", "Restaurant"},
    {2, "Pizza Point", "Restaurant"},
    {3, "Burger King", "Restaurant"},
    {4, "KFC", "Restaurant"},
    {5, "Subway", "Restaurant"},
    {6, "McDonalds", "Restaurant"},
    {7, "Bikanervala", "Restaurant"},
    {8, "Haldirams", "Restaurant"},
    {9, "Sector 62 Crossing", "Junction"},
    {10, "Sector 63 Crossing", "Junction"},
    {11, "Electronic City", "Commercial Area"},
    {12, "Main Road Junction", "Junction"},
    {13, "Market Crossing", "Junction"},
    {14, "Residential Area A", "Residential Area"},
    {15, "Residential Area B", "Residential Area"},
    {16, "Hospital Junction", "Junction"},
    {17, "Metro Station", "Transit Point"},
    {18, "Shopping Complex", "Commercial Area"},
    {19, "Customer Zone A", "Customer Location"},
    {20, "Customer Zone B", "Customer Location"},
    {21, "Customer Zone C", "Customer Location"},
    {22, "Industrial Outpost", "Disconnected Test Location"}
};

const LocationInfo* getLocationCatalog(int& count) {
    count = 23;
    return locations;
}

void buildCityMap(Graph& graph, const string& restaurantFile) {
    string restaurantNames[9];
    ifstream input(restaurantFile);
    string line;
    int index = 0;
    while (index < 9 && getline(input, line)) {
        if (!line.empty()) {
            restaurantNames[index] = line;
            index++;
        }
    }

    for (int i = 0; i < 9; i++) {
        if (!restaurantNames[i].empty()) {
            locations[i].name = restaurantNames[i];
        }
    }

    for (int i = 0; i < 23; i++) {
        graph.addVertex(locations[i].name);
    }

    int roads[][3] = {
        {0, 9, 4}, {0, 12, 7}, {1, 9, 5}, {1, 10, 3},
        {2, 10, 4}, {2, 11, 5}, {3, 11, 3}, {3, 13, 6},
        {4, 12, 4}, {4, 16, 6}, {5, 13, 3}, {5, 18, 5},
        {6, 12, 5}, {6, 17, 6}, {7, 13, 4}, {7, 14, 5},
        {8, 10, 5}, {8, 15, 6}, {9, 10, 2}, {9, 11, 6},
        {9, 12, 3}, {10, 13, 4}, {11, 12, 2}, {11, 17, 5},
        {12, 13, 3}, {12, 16, 5}, {13, 14, 4}, {13, 18, 3},
        {14, 15, 2}, {14, 19, 5}, {15, 16, 3}, {15, 20, 4},
        {16, 17, 2}, {16, 21, 6}, {17, 18, 2}, {18, 19, 3},
        {18, 20, 5}, {19, 21, 4}, {20, 21, 3}
    };

    int roadCount = sizeof(roads) / sizeof(roads[0]);
    for (int i = 0; i < roadCount; i++) {
        graph.addEdge(roads[i][0], roads[i][1], roads[i][2]);
    }
}
