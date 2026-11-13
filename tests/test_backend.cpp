#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cmath>

#include "storage/BPlusTree.h"
#include "storage/StorageManager.h"
#include "routing/Graph.h"
#include "routing/RouteManager.h"
#include "routing/CityData.h"
#include "search/Trie.h"
#include "search/SearchManager.h"
#include "dispatch/RiderManager.h"
#include "kitchen/KitchenManager.h"
#include "analytics/SegmentTree.h"
#include "analytics/AnalyticsManager.h"
#include "system/FoodDeliverySystem.h"

using namespace std;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "Assertion failed: " << msg << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(1); \
        } \
    } while (0)

void testBPlusTree() {
    std::cout << "[TEST] BPlusTree insert, search, deletion and rebalancing..." << std::endl;
    BPlusTree tree;

    for (int i = 1; i <= 20; i++) {
        tree.insert(i, static_cast<long long>(i * 100));
    }

    for (int i = 1; i <= 20; i++) {
        long long val = 0;
        TEST_ASSERT(tree.search(i, val) && val == i * 100, "BPlusTree search failed for key");
    }

    // Delete keys in order, forcing borrows and merges
    for (int i = 1; i <= 15; i++) {
        TEST_ASSERT(tree.remove(i), "BPlusTree remove failed");
        long long val = 0;
        TEST_ASSERT(!tree.search(i, val), "Removed key still found");
    }

    for (int i = 16; i <= 20; i++) {
        long long val = 0;
        TEST_ASSERT(tree.search(i, val) && val == i * 100, "Remaining key missing");
    }

    // Remove remaining keys down to empty
    for (int i = 16; i <= 20; i++) {
        TEST_ASSERT(tree.remove(i), "BPlusTree remove failed");
    }
    long long val = 0;
    TEST_ASSERT(!tree.search(16, val), "Empty tree found key");
    std::cout << "  -> BPlusTree tests passed." << std::endl;
}

void testGraphAndRouteManager() {
    std::cout << "[TEST] Graph with non-contiguous location IDs & RouteManager..." << std::endl;
    Graph g;
    // Non-contiguous location IDs: 100, 205, 301
    g.addVertex("Location A", 100);
    g.addVertex("Location B", 205);
    g.addVertex("Location C", 301);

    TEST_ASSERT(g.getVertexIndex(100) == 0, "Vertex index mapping 100 failed");
    TEST_ASSERT(g.getVertexIndex(205) == 1, "Vertex index mapping 205 failed");
    TEST_ASSERT(g.getVertexIndex(301) == 2, "Vertex index mapping 301 failed");
    TEST_ASSERT(g.getVertexExternalId(0) == 100, "Vertex external ID 0 failed");

    TEST_ASSERT(g.addEdge(100, 205, 5.0), "addEdge failed");
    TEST_ASSERT(g.addEdge(205, 301, 10.0), "addEdge failed");

    RouteManager rm(g);
    RouteResult r = rm.shortestPath(100, 301);
    TEST_ASSERT(r.routeExists, "Route should exist");
    TEST_ASSERT(std::abs(r.totalTravelTime - 15.0) < 1e-6, "Travel time incorrect");
    TEST_ASSERT(r.path.size() == 3, "Path size incorrect");
    TEST_ASSERT(r.path[0] == 100 && r.path[1] == 205 && r.path[2] == 301, "Path contains wrong external IDs");

    g.closeRoad(100, 205);
    RouteResult rClosed = rm.shortestPath(100, 301);
    TEST_ASSERT(!rClosed.routeExists, "Closed road should break route");

    g.reopenRoad(100, 205);
    RouteResult rReopened = rm.shortestPath(100, 301);
    TEST_ASSERT(rReopened.routeExists, "Reopened road should restore route");
    std::cout << "  -> Graph and RouteManager tests passed." << std::endl;
}

void testCityDataValidation() {
    std::cout << "[TEST] CityData error handling for malformed CSV..." << std::endl;
    std::string testDir = "tests/temp_test_data";
    std::filesystem::create_directories(testDir);

    // Valid locations
    std::ofstream locFile(testDir + "/locations.csv");
    locFile << "location_id,name,type\n0,Alpha,Hub\n1,Beta,Customer\n";
    locFile.close();

    // Invalid roads (malformed line with missing fields)
    std::ofstream roadFile(testDir + "/roads.csv");
    roadFile << "source,destination,time\n0,1,5\nINVALID_ROW_CORRUPT\n";
    roadFile.close();

    Graph g;
    TEST_ASSERT(!buildCityMap(g, testDir), "buildCityMap should fail on malformed roads CSV");

    std::filesystem::remove_all(testDir);
    std::cout << "  -> CityData validation tests passed." << std::endl;
}

void testSearchManager() {
    std::cout << "[TEST] SearchManager case-insensitivity and name preservation..." << std::endl;
    SearchManager sm;
    sm.addRestaurant("Pizza Hut");
    sm.addRestaurant("Dominos");

    TEST_ASSERT(sm.searchRestaurant("pizza hut"), "searchRestaurant case-insensitive failed");
    TEST_ASSERT(sm.searchRestaurant("PIZZA HUT"), "searchRestaurant upper-case failed");
    TEST_ASSERT(!sm.searchRestaurant("Burger King"), "Non-existent restaurant found");

    vector<string> autoRes = sm.getSuggestion("piz");
    TEST_ASSERT(!autoRes.empty(), "getSuggestion failed");
    TEST_ASSERT(autoRes[0] == "Pizza Hut", "Original casing not preserved");
    std::cout << "  -> SearchManager tests passed." << std::endl;
}

void testKitchenManager() {
    std::cout << "[TEST] KitchenManager station dispatch, peek and remove..." << std::endl;
    KitchenManager km;
    km.addOrder(1, 10, 15, 2);
    km.addOrder(2, 10, 25, 1);
    km.sendToStations();

    KitchenOrder frontOrder;
    bool peeked = km.peekStationOrder(0, frontOrder);
    TEST_ASSERT(peeked, "Should peek order at least loaded station");

    KitchenOrder removedOrder;
    bool removed = km.removeOrder(2, removedOrder);
    TEST_ASSERT(removed, "Should remove order 2 from kitchen stations");
    TEST_ASSERT(removedOrder.orderId == 2, "Removed wrong order");
    std::cout << "  -> KitchenManager tests passed." << std::endl;
}

void testSegmentTree() {
    std::cout << "[TEST] SegmentTree bounds checking and aggregation..." << std::endl;
    vector<double> values = {10.0, 20.0, 30.0, 40.0, 50.0};
    SegmentTree st;
    st.buildTree(values);

    TEST_ASSERT(std::abs(st.rangeSum(0, 4) - 150.0) < 1e-6, "Total sum incorrect");
    TEST_ASSERT(std::abs(st.rangeMax(0, 4) - 50.0) < 1e-6, "Max incorrect");
    TEST_ASSERT(std::abs(st.rangeSum(-5, 10) - 0.0) < 1e-6, "Out-of-bounds range query should return 0 safely");
    TEST_ASSERT(std::abs(st.rangeSum(3, 1) - 0.0) < 1e-6, "Inverted range sum should be 0");

    st.update(2, 100.0);
    TEST_ASSERT(std::abs(st.rangeSum(0, 4) - 220.0) < 1e-6, "Sum after update incorrect");
    TEST_ASSERT(std::abs(st.rangeMax(0, 4) - 100.0) < 1e-6, "Max after update incorrect");
    std::cout << "  -> SegmentTree tests passed." << std::endl;
}

void testStorageManager() {
    std::cout << "[TEST] StorageManager CRUD, verification and rebalance..." << std::endl;
    std::string testFolder = "tests/temp_storage_data";
    std::filesystem::remove_all(testFolder);

    StorageManager sm(testFolder);
    TEST_ASSERT(sm.initialize(), "StorageManager initialize failed");

    Restaurant r(1, "Test Bistro", 5);
    TEST_ASSERT(sm.insertRestaurant(r), "insertRestaurant failed");
    TEST_ASSERT(sm.restaurantExists(1), "restaurantExists failed");

    Restaurant retrieved;
    TEST_ASSERT(sm.getRestaurant(1, retrieved), "getRestaurant failed");
    TEST_ASSERT(retrieved.name == "Test Bistro" && retrieved.locationId == 5, "Restaurant mismatch");

    retrieved.locationId = 9;
    TEST_ASSERT(sm.updateRestaurant(retrieved), "updateRestaurant failed");

    Restaurant retrieved2;
    sm.getRestaurant(1, retrieved2);
    TEST_ASSERT(retrieved2.locationId == 9, "Updated location not saved");

    // Test setDataFolder index rebuilding (Issue 13)
    std::string testFolder2 = "tests/temp_storage_data2";
    std::filesystem::remove_all(testFolder2);
    sm.setDataFolder(testFolder2);
    TEST_ASSERT(!sm.restaurantExists(1), "New empty folder should not contain old index keys");

    std::filesystem::remove_all(testFolder);
    std::filesystem::remove_all(testFolder2);
    std::cout << "  -> StorageManager tests passed." << std::endl;
}

void testFoodDeliverySystemWorkflow() {
    std::cout << "[TEST] FoodDeliverySystem integration workflow & constraints..." << std::endl;
    // Clean any previous test data
    std::string testDataDir = "tests/temp_fds_data";
    std::filesystem::remove_all(testDataDir);
    std::filesystem::create_directories(testDataDir);

    // Create minimal data files
    {
        std::ofstream loc(testDataDir + "/locations.csv");
        loc << "location_id,name,type\n0,RestLocation,Commercial\n1,CustLocation,Residential\n2,UnreachableLocation,Outpost\n";
    }
    {
        std::ofstream rd(testDataDir + "/roads.csv");
        rd << "source,destination,time\n0,1,12.5\n"; // Only 0 and 1 connected
    }
    {
        std::ofstream rest(testDataDir + "/restaurants.csv");
        rest << "id,name,location_id\n1,TasteOfIndia,0\n";
    }
    {
        std::ofstream riders(testDataDir + "/riders.csv");
        riders << "rider_id,name,phone,location_id,status\nR1,John Doe,1234567890,0,Available\n";
    }

    FoodDeliverySystem fds(testDataDir);
    TEST_ASSERT(fds.isInitialized(), "FoodDeliverySystem initialization failed");

    // 1. Create order
    int orderId = fds.createOrder(1, 1, 1, 10);
    TEST_ASSERT(orderId > 0, "createOrder failed");
    TEST_ASSERT(fds.getOrderById(orderId) != nullptr, "Order not found");
    TEST_ASSERT(!fds.getOrderById(orderId)->prepared, "New order should not be prepared");
    TEST_ASSERT(!fds.getOrderById(orderId)->delivered, "New order should not be delivered");

    // Issue 6 test: completeOrder cannot deliver before prepared
    TEST_ASSERT(!fds.completeOrder(orderId), "completeOrder should fail when not prepared");

    // Issue 5 test: markOrderPrepared completes/removes from kitchen
    TEST_ASSERT(fds.markOrderPrepared(orderId), "markOrderPrepared failed");
    TEST_ASSERT(fds.getOrderById(orderId)->prepared, "Order should now be prepared");

    // Issue 7 test: Rider assignment requires a valid route
    // Test assigning rider to an order with unreachable customer location
    int badOrderId = fds.createOrder(1, 2, 1, 5); // Location 2 is disconnected
    TEST_ASSERT(badOrderId > 0, "createOrder for bad destination failed");
    fds.markOrderPrepared(badOrderId);
    TEST_ASSERT(!fds.assignRiderToOrder(badOrderId), "assignRiderToOrder must fail when route does not exist");

    // Assign rider to valid order
    TEST_ASSERT(fds.assignRiderToOrder(orderId), "assignRiderToOrder failed for valid order");
    Order *ord = fds.getOrderById(orderId);
    TEST_ASSERT(ord != nullptr && ord->riderAssigned, "Rider not marked assigned on order");
    TEST_ASSERT(ord->assignedRiderId == 1, "Assigned wrong rider");

    // Issue 9 test: Delivery persisted
    Delivery *del = fds.getDeliveryByOrderId(orderId);
    TEST_ASSERT(del != nullptr, "Delivery record missing");
    TEST_ASSERT(std::abs(del->travelTime - 12.5) < 1e-6, "Delivery travel time incorrect");

    // Complete order
    TEST_ASSERT(fds.completeOrder(orderId), "completeOrder failed");
    ord = fds.getOrderById(orderId);
    TEST_ASSERT(ord != nullptr && ord->delivered, "Order not marked delivered");
    del = fds.getDeliveryByOrderId(orderId);
    TEST_ASSERT(del != nullptr && del->completed, "Delivery not marked completed");

    // Issue 10 test: Restaurant location changes persisted
    TEST_ASSERT(fds.setRestaurantLocation(1, 1), "setRestaurantLocation failed");
    TEST_ASSERT(fds.getRestaurantLocation(1) == 1, "getRestaurantLocation did not return updated location");

    // Restart FoodDeliverySystem to test restoration from storage
    {
        FoodDeliverySystem fdsRestart(testDataDir);
        TEST_ASSERT(fdsRestart.isInitialized(), "Restarted FDS initialize failed");
        Order *restoredOrd = fdsRestart.getOrderById(orderId);
        TEST_ASSERT(restoredOrd != nullptr, "Restored order missing");
        TEST_ASSERT(restoredOrd->delivered, "Restored order should be delivered");
        TEST_ASSERT(fdsRestart.getRestaurantLocation(1) == 1, "Restored restaurant location mismatch");
        // Issue 4 test: delivered order should NOT be stuck in kitchen
        TEST_ASSERT(fdsRestart.getKitchenManager().pendingOrders() == 0, "Delivered orders should not be in kitchen heap");
    }

    std::filesystem::remove_all(testDataDir);
    std::cout << "  -> FoodDeliverySystem workflow tests passed." << std::endl;
}

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "RUNNING BACKEND AUTOMATED TEST SUITE" << std::endl;
    std::cout << "============================================" << std::endl;

    testBPlusTree();
    testGraphAndRouteManager();
    testCityDataValidation();
    testSearchManager();
    testKitchenManager();
    testSegmentTree();
    testStorageManager();
    testFoodDeliverySystemWorkflow();

    std::cout << "============================================" << std::endl;
    std::cout << "ALL BACKEND TESTS PASSED SUCCESSFULLY!" << std::endl;
    std::cout << "============================================" << std::endl;
    return 0;
}
