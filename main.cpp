#include "include/system/FoodDeliverySystem.h"
#include "include/ui/UIManager.h"
#include <QApplication>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::string dataDir = "data";
    if (argc > 1)
    {
        dataDir = argv[1];
    }

    std::cout << "Starting Food Delivery System Qt GUI...\n";
    FoodDeliverySystem system(dataDir);
    if (!system.isInitialized())
    {
        std::cerr << "Notice: Initialized with partial or default datasets from '" << dataDir << "'.\n";
    }

    UIManager ui(system);
    ui.show();

    return app.exec();
}
