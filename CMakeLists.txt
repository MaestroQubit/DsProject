
cmake_minimum_required(VERSION 3.16)
project(DsProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ds_core
    src/Routing/Graph.cpp
    src/Routing/MinHeap.cpp
    src/Routing/RouteManager.cpp
    src/search/Trie.cpp
    src/search/SearchManager.cpp
    src/dispatch/HashTable.cpp
    src/dispatch/MaxHeap.cpp
    src/dispatch/RiderManager.cpp
    src/kitchen/KitchenHeap.cpp
    src/kitchen/KitchenManager.cpp
    src/analytics/SegmentTree.cpp
    src/analytics/AnalyticsManager.cpp
    src/storage/BPlusTree.cpp
    src/storage/StorageManager.cpp
    src/system/FoodDeliverySystem.cpp
)

target_include_directories(ds_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(test_kitchen
    tests/test_kitchen.cpp
)

target_link_libraries(test_kitchen PRIVATE ds_core)

enable_testing()
add_test(NAME KitchenTest COMMAND test_kitchen)
