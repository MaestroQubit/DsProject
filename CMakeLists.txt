cmake_minimum_required(VERSION 3.16)
project(DsProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

# Core data structures and managers
add_library(ds_core
    src/Routing/Graph.cpp
    src/Routing/MinHeap.cpp
    src/Routing/RouteManager.cpp
    src/Routing/CityData.cpp
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

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Production Qt GUI Executable
add_executable(FoodDeliveryApp
    main.cpp
    src/ui/UIManager.cpp
    include/ui/UIManager.h
)

target_include_directories(FoodDeliveryApp PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(FoodDeliveryApp PRIVATE ds_core)

if (Qt6_FOUND)
    target_link_libraries(FoodDeliveryApp PRIVATE Qt6::Widgets)
else()
    target_link_libraries(FoodDeliveryApp PRIVATE Qt5::Widgets)
endif()

# Automated testing
enable_testing()

add_executable(backend_tests
    tests/test_backend.cpp
)
target_link_libraries(backend_tests PRIVATE ds_core)
add_test(NAME BackendTests COMMAND backend_tests)
