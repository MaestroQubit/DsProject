QT += core gui widgets

CONFIG += c++17

TEMPLATE = app
TARGET = FoodDeliveryApp

INCLUDEPATH += include

HEADERS += \
    include/models/Restaurant.h \
    include/models/Rider.h \
    include/models/Order.h \
    include/models/Delivery.h \
    include/routing/Graph.h \
    include/routing/MinHeap.h \
    include/routing/RouteManager.h \
    include/routing/CityData.h \
    include/search/Trie.h \
    include/search/SearchManager.h \
    include/dispatch/HashTable.h \
    include/dispatch/MaxHeap.h \
    include/dispatch/RiderManager.h \
    include/kitchen/KitchenHeap.h \
    include/kitchen/KitchenManager.h \
    include/analytics/SegmentTree.h \
    include/analytics/AnalyticsManager.h \
    include/analytics/AnalyticsData.h \
    include/storage/BPlusTree.h \
    include/storage/StorageManager.h \
    include/system/FoodDeliverySystem.h \
    include/ui/UIManager.h \
    include/ui/MainWindow.h

SOURCES += \
    src/Routing/Graph.cpp \
    src/Routing/MinHeap.cpp \
    src/Routing/RouteManager.cpp \
    src/Routing/CityData.cpp \
    src/search/Trie.cpp \
    src/search/SearchManager.cpp \
    src/dispatch/HashTable.cpp \
    src/dispatch/MaxHeap.cpp \
    src/dispatch/RiderManager.cpp \
    src/kitchen/KitchenHeap.cpp \
    src/kitchen/KitchenManager.cpp \
    src/analytics/SegmentTree.cpp \
    src/analytics/AnalyticsManager.cpp \
    src/storage/BPlusTree.cpp \
    src/storage/StorageManager.cpp \
    src/system/FoodDeliverySystem.cpp \
    src/ui/UIManager.cpp \
    main.cpp
