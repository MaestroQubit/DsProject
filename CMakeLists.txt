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
)
target_include_directories(ds_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
enable_testing()
add_executable(routing_tests
    tests/testRouting/DijkstraTests.cpp
    tests/testRouting/GraphTests.cpp
    tests/testRouting/MinHeapTests.cpp
)
target_link_libraries(routing_tests PRIVATE ds_core)
add_test(NAME routing_tests COMMAND routing_tests)
add_executable(trie_tests tests/testTrie.cpp)
target_link_libraries(trie_tests PRIVATE ds_core)
add_test(NAME trie_tests COMMAND trie_tests)
add_executable(search_manager_tests tests/testSearchManager.cpp)
target_link_libraries(search_manager_tests PRIVATE ds_core)
add_test(NAME search_manager_tests COMMAND search_manager_tests)