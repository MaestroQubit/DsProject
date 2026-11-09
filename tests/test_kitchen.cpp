#include "kitchen/KitchenManager.h"
#include <iostream>

int main() {
    KitchenManager kitchen;

    kitchen.addOrder(1, 101, 20, 1);
    kitchen.addOrder(2, 101, 10, 3);
    kitchen.addOrder(3, 102, 30, 5);
    kitchen.addOrder(4, 102, 15, 2);
    kitchen.addOrder(5, 103, 25, 1);

    std::cout << "Pending orders: " << kitchen.pendingOrders() << "\n\n";

    kitchen.sendToStations();
    std::cout << "\n";
    kitchen.showStatus();

    std::cout << "\nCooking started\n";
    for (int i = 0; i < kitchen.stationCount(); i++) {
        KitchenOrder ready;
        while (kitchen.completeOrder(i, ready)) {
            std::cout << "Cook " << i + 1 << " prepared Order " << ready.orderId << "\n";
        }
    }

    return 0;
}
