# Campus & City Food Delivery Management System

A high-performance, modular desktop food delivery management system developed in C++17 with a dedicated **Qt graphical user interface (GUI)**. The system is designed from the ground up using custom data structures, coordinated domain managers, a centralized system engine, and a multi-tab desktop graphical dashboard.

---

## Table of Contents

- [Architectural Overview](#architectural-overview)
- [Core Modules & Custom Data Structures](#core-modules--custom-data-structures)
- [Graphical User Interface (GUI) Features](#graphical-user-interface-gui-features)
- [System Requirements](#system-requirements)
- [How to Build and Run](#how-to-build-and-run)
  - [Method 1: One-Click Scripts (Recommended)](#method-1-one-click-scripts-recommended)
  - [Method 2: CMake + Ninja (Command Line)](#method-2-cmake--ninja-command-line)
  - [Method 3: Qt Creator (IDE)](#method-3-qt-creator-ide)
- [Running Automated Backend Tests](#running-automated-backend-tests)
- [Project Directory Structure](#project-directory-structure)
- [Data Directory & Files](#data-directory--files)

---

## Architectural Overview

The application follows a strictly layered, decoupled software architecture:

```
┌──────────────────────────────────────────────────────────────────────────────────┐
│                             Main Application (main.cpp)                          │
│                   Initializes QApplication and launches UIManager               │
└────────────────────────────────────────┬─────────────────────────────────────────┘
                                         │
┌────────────────────────────────────────▼─────────────────────────────────────────┐
│                               UI Module (UIManager)                              │
│         Qt6/Qt5 Multi-Tab GUI, 2D Map Canvas, Real-Time Tables & Forms           │
└────────────────────────────────────────┬─────────────────────────────────────────┘
                                         │
┌────────────────────────────────────────▼─────────────────────────────────────────┐
│                    Central Integration (FoodDeliverySystem)                     │
│       Coordinates workflow across all domain managers & synchronizes state       │
└───────┬─────────────┬─────────────┬─────────────┬──────────────┬─────────────┬───┘
        │             │             │             │              │             │
┌───────▼──────┐┌─────▼──────┐┌─────▼──────┐┌─────▼───────┐┌─────▼──────┐┌─────▼──────┐
│SearchManager ││RouteManager││RiderManager││KitchenManage││AnalyticsMgr││StorageManage│
│   (Search)   ││  (Routing) ││ (Dispatch) ││  (Cooking)  ││ (Metrics)  ││(Persistence)│
└───────┬──────┘└─────┬──────┘└─────┬──────┘└─────┬───────┘└─────┬──────┘└─────┬──────┘
        │             │             │             │              │             │
┌───────▼──────┐┌─────▼──────┐┌─────▼──────┐┌─────▼───────┐┌─────▼──────┐┌─────▼──────┐
│     Trie     ││Graph+MinHea││MaxHeap+Hash││KitchenHeap  ││SegmentTree ││  BPlusTree  │
│ (Prefix Tree)││ (Dijkstra) ││(Rider Queue││(StationQueue││(Range Tree)││ (Index/Disk)│
└──────────────┘└────────────┘└────────────┘└─────────────┘└────────────┘└────────────┘
```

1. **Custom Data Structures**: Low-level, memory-efficient data structure implementations written in modern C++.
2. **Managers Layer**: Specialized business logic controllers wrapping data structures to manage domain actions.
3. **FoodDeliverySystem**: Central facade that coordinates order life cycles, rider allocations, kitchen workflows, and persistent storage.
4. **UI Layer (UIManager / MainWindow)**: Dedicated desktop Qt GUI providing visual controls, tables, and an interactive 2D map.
5. **Main Entry Point (`main.cpp`)**: Application bootstrap linking the Qt event loop with the system engine.

---

## Core Modules & Custom Data Structures

### 1. Search Module
- **Data Structure**: `Trie` ([include/search/Trie.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/search/Trie.h))
- **Manager**: `SearchManager` ([include/search/SearchManager.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/search/SearchManager.h))
- **Capabilities**: Prefix-based autocomplete search with $O(L)$ time complexity, case-insensitive string search, and full restaurant metadata cataloging.

### 2. Routing & Navigation Module
- **Data Structures**: `Graph` ([include/routing/Graph.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/routing/Graph.h)) and `MinHeap` ([include/routing/MinHeap.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/routing/MinHeap.h))
- **Manager**: `RouteManager` ([include/routing/RouteManager.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/routing/RouteManager.h))
- **Capabilities**: Weighted adjacency list graph with support for non-contiguous vertex IDs, dynamic traffic weight adjustments, road closures/reopenings, and Dijkstra's algorithm for shortest-path route computation.

### 3. Rider Dispatch Module
- **Data Structures**: `MaxHeap` ([include/dispatch/MaxHeap.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/dispatch/MaxHeap.h)) and `HashTable` ([include/dispatch/HashTable.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/dispatch/HashTable.h))
- **Manager**: `RiderManager` ([include/dispatch/RiderManager.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/dispatch/RiderManager.h))
- **Capabilities**: Fast $O(1)$ rider lookups via chained hashing, priority ranking of available delivery riders using composite scores (rating and distance), and automatic dispatch.

### 4. Kitchen & Order Pipeline Module
- **Data Structures**: `KitchenHeap` ([include/kitchen/KitchenHeap.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/kitchen/KitchenHeap.h)) and `OrderQueue` ([include/kitchen/KitchenManager.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/kitchen/KitchenManager.h))
- **Manager**: `KitchenManager` ([include/kitchen/KitchenManager.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/kitchen/KitchenManager.h))
- **Capabilities**: Priority-based heap queue sorting incoming orders by preparation urgency, dynamic load-balancing dispatch across three cooking stations (Station 1, 2, 3), and FIFO cooking progression.

### 5. Analytics Module
- **Data Structure**: `SegmentTree` ([include/analytics/SegmentTree.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/analytics/SegmentTree.h))
- **Manager**: `AnalyticsManager` ([include/analytics/AnalyticsManager.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/analytics/AnalyticsManager.h))
- **Capabilities**: Segment tree for range-based operational telemetry, throughput calculation, peak load monitoring, and fleet rating distribution.

### 6. Storage & Persistence Module
- **Data Structure**: `BPlusTree` ([include/storage/BPlusTree.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/storage/BPlusTree.h))
- **Manager**: `StorageManager` ([include/storage/StorageManager.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/storage/StorageManager.h))
- **Capabilities**: In-memory and on-disk indexed storage for restaurants, menu items, orders, deliveries, and riders. Supports fast key search, sorted range traversal, node splitting/rebalancing, and CSV/binary synchronization.

---

## Graphical User Interface (GUI) Features

The application runs in a standalone desktop window ([include/ui/UIManager.h](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/include/ui/UIManager.h)) with six interactive tabs:

1. **Restaurants Tab**:
   - Live autocomplete search field powered by the `Trie`.
   - Comprehensive tabular display of registered restaurants and their node locations.
   - Form to add new restaurants with validation.
2. **Orders Tab**:
   - Real-time orders table displaying Order ID, Restaurant ID, Customer Location, Priority, Prep Time, and Lifecycle Status (`Pending` $\rightarrow$ `Prepared` $\rightarrow$ `In Transit` $\rightarrow$ `Delivered`).
   - Order creation form.
   - Action controls to mark orders prepared or complete delivery.
3. **Kitchen Stations Tab**:
   - Status indicators and workload meters for Cooking Stations 1, 2, and 3.
   - Station dispatch button to transfer pending orders from the `KitchenHeap` to station queues.
   - Individual station cooking completion triggers.
4. **Riders Fleet Tab**:
   - Fleet overview table showing Rider ID, Name, Rating, Status (`Available`, `Busy`, `Offline`), and Current Location.
   - Rider registration and update controls.
   - One-click auto-dispatch button finding and assigning the optimal rider using `MaxHeap` and `Dijkstra`.
5. **City Map Tab**:
   - Custom double-buffered 2D canvas (`MapViewWidget`) rendering the 23-location city graph and road segments.
   - Source and destination input fields to compute shortest paths.
   - Visual route overlay highlighting the calculated path in green with estimated travel time.
6. **Analytics & Reports Tab**:
   - Dashboard cards detailing total processed orders, active orders, completed deliveries, fleet counts, and average ratings.
   - Instant metrics refresh button.

---

## System Requirements

To compile and run this project, ensure you have the following installed on your machine:

| Component | Minimum Version | Notes |
| :--- | :--- | :--- |
| **Operating System** | Windows 10/11, Linux, or macOS | Configured and tested on Windows 11 64-bit |
| **C++ Compiler** | C++17 compliant (GCC 9+, MinGW-w64, Clang 10+, or MSVC 2019+) | UCRT64 GCC 16.x or 11+ recommended |
| **Qt Framework** | Qt 6 (6.2+) or Qt 5 (5.15+) | Specifically `QtCore`, `QtGui`, `QtWidgets` |
| **Build Tools** | CMake 3.16+ and Ninja (or `make` / `mingw32-make`) | Installed via MSYS2 or system package manager |

### Installing Requirements on Windows (via MSYS2)

If using MSYS2 UCRT64:
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
```

Ensure `C:\msys64\ucrt64\bin` is added to your Windows system `PATH`.

---

## How to Build and Run

### Method 1: One-Click Scripts (Recommended)

The repository provides pre-configured scripts that automatically detect CMake/Ninja, compile all modules, and launch the GUI window.

#### In PowerShell:
```powershell
.\run.ps1
```

#### In Command Prompt (CMD):
```cmd
build.bat
```

---

### Method 2: CMake + Ninja (Command Line)

You can build the project directly using CMake from the repository root:

1. **Configure the build**:
   ```bash
   cmake -B build -G Ninja
   ```
2. **Compile the executable**:
   ```bash
   cmake --build build --target FoodDeliveryApp
   ```
3. **Run the application**:
   ```bash
   cd build
   ./FoodDeliveryApp.exe
   ```

*(Note: On Windows, run `FoodDeliveryApp.exe` from inside the `build` directory so it accesses the copied `data/` folder).*

---

### Method 3: Qt Creator (IDE)

If you prefer developing in Qt Creator:
1. Launch **Qt Creator**.
2. Select **File** $\rightarrow$ **Open File or Project...** (or `Ctrl + O`).
3. Select [DsProject.pro](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/DsProject.pro).
4. Select your **Desktop Qt 6.x / Qt 5.x MinGW 64-bit** kit and click **Configure Project**.
5. Press the green **Run** button (or `Ctrl + R`).

---

## Running Automated Backend Tests

The project includes an automated test suite ([tests/test_backend.cpp](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/tests/test_backend.cpp)) verifying all custom data structures, invariants, edge cases, and manager interactions.

### Building & Running Tests via CMake:
```bash
cmake -B build -G Ninja
cmake --build build --target backend_tests
./build/backend_tests.exe
```

### Direct Compilation with GCC:
```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_backend.cpp src/Routing/*.cpp src/search/*.cpp src/dispatch/*.cpp src/kitchen/*.cpp src/analytics/*.cpp src/storage/*.cpp src/system/*.cpp -o backend_tests.exe
.\backend_tests.exe
```

All 8 test suites (B+ Tree, Graph/RouteManager, CityData, SearchManager/Trie, KitchenManager, SegmentTree, StorageManager, and System Integration) run and print comprehensive verification logs.

---

## Project Directory Structure

```
DsProject/
├── .vscode/                 # VS Code build tasks & settings
├── build/                   # CMake output directory (generated on build)
├── data/                    # Dataset files (CSV format)
│   ├── customers.csv        # Customer accounts and default locations
│   ├── locations.csv        # City node coordinates & landmark names
│   ├── orders.csv           # Historical order dataset
│   ├── restaurants.csv      # Restaurant profiles & location bindings
│   ├── riders.csv           # Fleet profiles, ratings, and locations
│   └── roads.csv            # Road network edges and travel times
├── include/                 # Public header files
│   ├── analytics/           # AnalyticsManager, SegmentTree, AnalyticsData
│   ├── dispatch/            # RiderManager, MaxHeap, HashTable
│   ├── kitchen/             # KitchenManager, KitchenHeap
│   ├── models/              # Order, Rider, Restaurant, Delivery, MenuItem
│   ├── routing/             # RouteManager, Graph, MinHeap, CityData, RouteResult
│   ├── search/              # SearchManager, Trie
│   ├── storage/             # StorageManager, BPlusTree
│   ├── system/              # FoodDeliverySystem central coordinator
│   └── ui/                  # UIManager, MainWindow Qt GUI headers
├── src/                     # C++ source implementations
│   ├── Routing/             # Graph, MinHeap, RouteManager, CityData
│   ├── analytics/           # AnalyticsManager, SegmentTree
│   ├── dispatch/            # RiderManager, MaxHeap, HashTable
│   ├── kitchen/             # KitchenManager, KitchenHeap
│   ├── search/              # SearchManager, Trie
│   ├── storage/             # StorageManager, BPlusTree
│   ├── system/              # FoodDeliverySystem
│   └── ui/                  # UIManager (Qt MainWindow & MapViewWidget)
├── tests/                   # Automated backend test suite
│   └── test_backend.cpp     # Comprehensive test cases
├── build.bat                # Windows Command Prompt build & run script
├── CMakeLists.txt           # CMake build specification (Qt6/Qt5 + MOC)
├── DsProject.pro            # Qt Creator / qmake project file
├── main.cpp                 # Application entry point (Qt bootstrap)
├── README.md                # Project documentation
└── run.ps1                  # PowerShell automated build & run script
```

---

## Data Directory & Files

The [data/](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/data) folder contains the initial seed data automatically parsed on application startup:

- **[locations.csv](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/data/locations.csv)**: Defines graph vertices with location IDs, names, and campus zone references.
- **[roads.csv](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/data/roads.csv)**: Defines bidirectional weighted edges representing street network travel times (minutes).
- **[restaurants.csv](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/data/restaurants.csv)**: Restaurant names mapped to city node IDs.
- **[riders.csv](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/data/riders.csv)**: Rider IDs, names, initial ratings, availability states, and locations.
- **[orders.csv](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/data/orders.csv)**: Seed orders with restaurant IDs, customer destination IDs, priority ratings, and cooking durations.
- **[customers.csv](file:///c:/Users/Asus/OneDrive/Desktop/Coding/VSCode-Insider/DsProject/data/customers.csv)**: Customer profiles and delivery address nodes.
