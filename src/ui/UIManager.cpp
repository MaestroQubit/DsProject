#include "../../include/ui/UIManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <sstream>
#include <iomanip>

static struct NodePos {
    int x;
    int y;
} g_qtNodePositions[25] = {
    {60, 60},   {160, 50},  {260, 50},  {360, 60},  {460, 60},
    {60, 160},  {160, 150}, {260, 150}, {360, 160}, {120, 240},
    {220, 240}, {320, 240}, {180, 320}, {280, 320}, {80, 390},
    {180, 400}, {280, 400}, {380, 390}, {460, 320}, {80, 480},
    {240, 480}, {400, 480}, {520, 480}, {0, 0},    {0, 0}
};

MapViewWidget::MapViewWidget(FoodDeliverySystem &fds, QWidget *parent)
    : QWidget(parent), system(fds)
{
    setMinimumSize(600, 560);
    setStyleSheet("background-color: #f8f9fa; border: 1px solid #ced4da; border-radius: 6px;");
}

void MapViewWidget::setRouteResult(const RouteResult &res)
{
    lastRouteResult = res;
    update();
}

void MapViewWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int vertexCount = system.getGraph().getVertexCount();

    // Draw default road network edges
    QPen defaultRoadPen(QColor(180, 185, 190), 2, Qt::SolidLine);
    painter.setPen(defaultRoadPen);

    for (int u = 0; u < vertexCount && u < 23; u++)
    {
        const Graph::EdgeNode *edge = system.getGraph().getAdjacencyHead(u);
        while (edge)
        {
            int v = edge->destination;
            if (v < 23 && u < v)
            {
                painter.drawLine(g_qtNodePositions[u].x + 30, g_qtNodePositions[u].y + 20,
                                 g_qtNodePositions[v].x + 30, g_qtNodePositions[v].y + 20);
            }
            edge = edge->next;
        }
    }

    // Highlight active route calculated via Dijkstra's algorithm
    if (lastRouteResult.routeExists && lastRouteResult.path.size() >= 2)
    {
        QPen activeRoutePen(QColor(46, 204, 113), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(activeRoutePen);
        for (size_t i = 0; i + 1 < lastRouteResult.path.size(); i++)
        {
            int u = system.getGraph().getVertexIndex(lastRouteResult.path[i]);
            int v = system.getGraph().getVertexIndex(lastRouteResult.path[i + 1]);
            if (u >= 0 && u < 23 && v >= 0 && v < 23)
            {
                painter.drawLine(g_qtNodePositions[u].x + 30, g_qtNodePositions[u].y + 20,
                                 g_qtNodePositions[v].x + 30, g_qtNodePositions[v].y + 20);
            }
        }
    }

    // Draw graph location nodes
    QFont font("Segoe UI", 9, QFont::Bold);
    painter.setFont(font);

    for (int i = 0; i < vertexCount && i < 23; i++)
    {
        int extId = system.getGraph().getVertexExternalId(i);
        int cx = g_qtNodePositions[i].x + 30;
        int cy = g_qtNodePositions[i].y + 20;

        bool onPath = false;
        if (lastRouteResult.routeExists)
        {
            for (int p : lastRouteResult.path)
            {
                if (p == extId) { onPath = true; break; }
            }
        }

        if (onPath)
        {
            painter.setBrush(QBrush(QColor(46, 204, 113)));
            painter.setPen(QPen(QColor(39, 174, 96), 2));
        }
        else
        {
            painter.setBrush(QBrush(QColor(52, 152, 219)));
            painter.setPen(QPen(QColor(41, 128, 185), 2));
        }

        painter.drawEllipse(QPoint(cx, cy), 16, 16);

        painter.setPen(Qt::white);
        QString text = QString::number(extId);
        QRect textRect(cx - 16, cy - 16, 32, 32);
        painter.drawText(textRect, Qt::AlignCenter, text);
    }
}

UIManager::UIManager(FoodDeliverySystem &fds, QWidget *parent)
    : QMainWindow(parent), system(fds)
{
    setWindowTitle("Campus & City Food Delivery System - Qt GUI");
    resize(1050, 720);
    setupUi();
    refreshAll();
}

void UIManager::run()
{
    show();
}

void UIManager::setupUi()
{
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    tabWidget->addTab(createRestaurantTab(), "Restaurants");
    tabWidget->addTab(createOrderTab(), "Orders");
    tabWidget->addTab(createKitchenTab(), "Kitchen Stations");
    tabWidget->addTab(createRiderTab(), "Riders Fleet");
    tabWidget->addTab(createMapTab(), "City Map");
    tabWidget->addTab(createAnalyticsTab(), "Analytics & Reports");
}

QWidget *UIManager::createRestaurantTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);

    // Search bar
    QHBoxLayout *searchLayout = new QHBoxLayout();
    editSearchRest = new QLineEdit();
    editSearchRest->setPlaceholderText("Search restaurant prefix (Trie Autocomplete)...");
    QPushButton *btnSearch = new QPushButton("Search / Autocomplete");
    QPushButton *btnResetSearch = new QPushButton("Show All");
    searchLayout->addWidget(editSearchRest);
    searchLayout->addWidget(btnSearch);
    searchLayout->addWidget(btnResetSearch);
    mainLayout->addLayout(searchLayout);

    // Restaurants Table
    tableRestaurants = new QTableWidget();
    tableRestaurants->setColumnCount(3);
    tableRestaurants->setHorizontalHeaderLabels({"Restaurant ID", "Name", "Location ID"});
    tableRestaurants->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableRestaurants->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableRestaurants->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(tableRestaurants);

    // Add Restaurant Form
    QHBoxLayout *formLayout = new QHBoxLayout();
    editRestName = new QLineEdit();
    editRestName->setPlaceholderText("Restaurant Name");
    editRestLoc = new QLineEdit();
    editRestLoc->setPlaceholderText("Location ID (0-22)");
    btnAddRest = new QPushButton("Add Restaurant");
    formLayout->addWidget(editRestName);
    formLayout->addWidget(editRestLoc);
    formLayout->addWidget(btnAddRest);
    mainLayout->addLayout(formLayout);

    connect(btnSearch, &QPushButton::clicked, this, &UIManager::onSearchRestaurants);
    connect(btnResetSearch, &QPushButton::clicked, this, &UIManager::refreshAll);
    connect(btnAddRest, &QPushButton::clicked, this, &UIManager::onAddRestaurant);

    return tab;
}

QWidget *UIManager::createOrderTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);

    tableOrders = new QTableWidget();
    tableOrders->setColumnCount(6);
    tableOrders->setHorizontalHeaderLabels({"Order ID", "Rest ID", "Customer Loc", "Priority", "Prep Time", "Status"});
    tableOrders->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableOrders->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableOrders->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(tableOrders);

    // Create Order controls
    QHBoxLayout *createLayout = new QHBoxLayout();
    editOrderRestId = new QLineEdit();
    editOrderRestId->setPlaceholderText("Rest ID");
    editOrderCustLoc = new QLineEdit();
    editOrderCustLoc->setPlaceholderText("Cust Loc (0-22)");
    editOrderPrio = new QLineEdit();
    editOrderPrio->setPlaceholderText("Priority (1-10)");
    editOrderPrepTime = new QLineEdit();
    editOrderPrepTime->setPlaceholderText("Prep Time (min)");
    btnCreateOrder = new QPushButton("Create Order");
    createLayout->addWidget(editOrderRestId);
    createLayout->addWidget(editOrderCustLoc);
    createLayout->addWidget(editOrderPrio);
    createLayout->addWidget(editOrderPrepTime);
    createLayout->addWidget(btnCreateOrder);
    mainLayout->addLayout(createLayout);

    // Order Action controls
    QHBoxLayout *actionLayout = new QHBoxLayout();
    editMarkOrderId = new QLineEdit();
    editMarkOrderId->setPlaceholderText("Order ID");
    btnMarkPrepared = new QPushButton("Mark Prepared");
    editDeliverOrderId = new QLineEdit();
    editDeliverOrderId->setPlaceholderText("Order ID");
    btnCompleteDelivery = new QPushButton("Complete Delivery");

    actionLayout->addWidget(editMarkOrderId);
    actionLayout->addWidget(btnMarkPrepared);
    actionLayout->addSpacing(20);
    actionLayout->addWidget(editDeliverOrderId);
    actionLayout->addWidget(btnCompleteDelivery);
    mainLayout->addLayout(actionLayout);

    connect(btnCreateOrder, &QPushButton::clicked, this, &UIManager::onCreateOrder);
    connect(btnMarkPrepared, &QPushButton::clicked, this, &UIManager::onMarkOrderPrepared);
    connect(btnCompleteDelivery, &QPushButton::clicked, this, &UIManager::onCompleteDelivery);

    return tab;
}

QWidget *UIManager::createKitchenTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);

    tableKitchen = new QTableWidget();
    tableKitchen->setColumnCount(4);
    tableKitchen->setHorizontalHeaderLabels({"Station #", "Current Order ID", "Station Workload", "Action Status"});
    tableKitchen->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableKitchen->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableKitchen->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(tableKitchen);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnSendToStations = new QPushButton("Dispatch Orders From Queue to Stations");
    btnCompleteStation1 = new QPushButton("Complete Station 1 Order");
    btnCompleteStation2 = new QPushButton("Complete Station 2 Order");
    btnCompleteStation3 = new QPushButton("Complete Station 3 Order");

    btnLayout->addWidget(btnSendToStations);
    btnLayout->addWidget(btnCompleteStation1);
    btnLayout->addWidget(btnCompleteStation2);
    btnLayout->addWidget(btnCompleteStation3);
    mainLayout->addLayout(btnLayout);

    connect(btnSendToStations, &QPushButton::clicked, this, &UIManager::onSendToStations);
    connect(btnCompleteStation1, &QPushButton::clicked, [this]() { onCompleteStationOrder(0); });
    connect(btnCompleteStation2, &QPushButton::clicked, [this]() { onCompleteStationOrder(1); });
    connect(btnCompleteStation3, &QPushButton::clicked, [this]() { onCompleteStationOrder(2); });

    return tab;
}

QWidget *UIManager::createRiderTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);

    tableRiders = new QTableWidget();
    tableRiders->setColumnCount(5);
    tableRiders->setHorizontalHeaderLabels({"Rider ID", "Name", "Rating", "Status", "Current Loc"});
    tableRiders->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableRiders->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableRiders->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(tableRiders);

    // Add / Update Rider
    QHBoxLayout *addLayout = new QHBoxLayout();
    editRiderId = new QLineEdit();
    editRiderId->setPlaceholderText("Rider ID");
    editRiderName = new QLineEdit();
    editRiderName->setPlaceholderText("Name");
    editRiderRating = new QLineEdit();
    editRiderRating->setPlaceholderText("Rating (1.0-5.0)");
    editRiderLoc = new QLineEdit();
    editRiderLoc->setPlaceholderText("Location ID");
    btnAddRider = new QPushButton("Add / Update Rider");

    addLayout->addWidget(editRiderId);
    addLayout->addWidget(editRiderName);
    addLayout->addWidget(editRiderRating);
    addLayout->addWidget(editRiderLoc);
    addLayout->addWidget(btnAddRider);
    mainLayout->addLayout(addLayout);

    // Auto-dispatch optimal rider
    QHBoxLayout *dispatchLayout = new QHBoxLayout();
    editAssignOrderId = new QLineEdit();
    editAssignOrderId->setPlaceholderText("Order ID to Dispatch");
    btnAutoAssignRider = new QPushButton("Auto-Assign Optimal Rider (Max-Heap + Dijkstra)");

    dispatchLayout->addWidget(editAssignOrderId);
    dispatchLayout->addWidget(btnAutoAssignRider);
    mainLayout->addLayout(dispatchLayout);

    connect(btnAddRider, &QPushButton::clicked, this, &UIManager::onAddRider);
    connect(btnAutoAssignRider, &QPushButton::clicked, this, &UIManager::onAutoAssignRider);

    return tab;
}

QWidget *UIManager::createMapTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);

    QHBoxLayout *topLayout = new QHBoxLayout();
    editRouteSrc = new QLineEdit();
    editRouteSrc->setPlaceholderText("Source Location ID (0-22)");
    editRouteDest = new QLineEdit();
    editRouteDest->setPlaceholderText("Destination Location ID (0-22)");
    btnFindRoute = new QPushButton("Calculate Shortest Route (Dijkstra)");

    topLayout->addWidget(editRouteSrc);
    topLayout->addWidget(editRouteDest);
    topLayout->addWidget(btnFindRoute);
    mainLayout->addLayout(topLayout);

    lblRouteSummary = new QLabel("Enter start and destination locations to view Dijkstra shortest route.");
    lblRouteSummary->setStyleSheet("font-weight: bold; color: #2c3e50; padding: 4px;");
    mainLayout->addWidget(lblRouteSummary);

    mapView = new MapViewWidget(system, tab);
    mainLayout->addWidget(mapView);

    connect(btnFindRoute, &QPushButton::clicked, this, &UIManager::onFindRoute);

    return tab;
}

QWidget *UIManager::createAnalyticsTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(tab);

    lblAnalyticsCards = new QLabel();
    lblAnalyticsCards->setStyleSheet(
        "background-color: #ecf0f1; border: 1px solid #bdc3c7; border-radius: 8px; padding: 20px; font-size: 14px;"
    );
    mainLayout->addWidget(lblAnalyticsCards);

    QPushButton *btnRefresh = new QPushButton("Refresh Analytics Metrics");
    mainLayout->addWidget(btnRefresh);

    connect(btnRefresh, &QPushButton::clicked, this, &UIManager::refreshAll);

    return tab;
}

void UIManager::refreshAll()
{
    // 1. Restaurants
    tableRestaurants->setRowCount(0);
    const auto &allRests = system.getSearchManager().getAllRestaurants();
    for (const auto &r : allRests)
    {
        int row = tableRestaurants->rowCount();
        tableRestaurants->insertRow(row);
        tableRestaurants->setItem(row, 0, new QTableWidgetItem(QString::number(r.id)));
        tableRestaurants->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r.name)));
        tableRestaurants->setItem(row, 2, new QTableWidgetItem(QString::number(r.locationId)));
    }

    // 2. Orders
    tableOrders->setRowCount(0);
    const auto &orders = system.getOrders();
    for (const auto &o : orders)
    {
        int row = tableOrders->rowCount();
        tableOrders->insertRow(row);
        tableOrders->setItem(row, 0, new QTableWidgetItem(QString::number(o.orderId)));
        tableOrders->setItem(row, 1, new QTableWidgetItem(QString::number(o.restaurantId)));
        tableOrders->setItem(row, 2, new QTableWidgetItem(QString::number(o.customerLocationId)));
        tableOrders->setItem(row, 3, new QTableWidgetItem(QString::number(o.priority)));
        tableOrders->setItem(row, 4, new QTableWidgetItem(QString::number(o.prepTime) + " min"));
        std::string status = o.delivered ? "Delivered" : (o.riderAssigned ? "In Transit" : (o.prepared ? "Prepared" : "Pending"));
        tableOrders->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(status)));
    }

    // 3. Kitchen Stations
    tableKitchen->setRowCount(0);
    int numStations = system.getKitchenManager().stationCount();
    for (int i = 0; i < numStations; i++)
    {
        int row = tableKitchen->rowCount();
        tableKitchen->insertRow(row);
        tableKitchen->setItem(row, 0, new QTableWidgetItem("Station " + QString::number(i + 1)));
        
        KitchenOrder ko;
        if (system.getKitchenManager().peekStationOrder(i, ko))
        {
            tableKitchen->setItem(row, 1, new QTableWidgetItem(QString::number(ko.orderId)));
            tableKitchen->setItem(row, 2, new QTableWidgetItem(QString::number(ko.prepTime) + " min"));
            tableKitchen->setItem(row, 3, new QTableWidgetItem("Cooking"));
        }
        else
        {
            tableKitchen->setItem(row, 1, new QTableWidgetItem("Idle"));
            tableKitchen->setItem(row, 2, new QTableWidgetItem("0 min"));
            tableKitchen->setItem(row, 3, new QTableWidgetItem("Available"));
        }
    }

    // 4. Riders
    tableRiders->setRowCount(0);
    const auto &riders = system.getRiderManager().getAllRiders();
    for (const auto &rd : riders)
    {
        int row = tableRiders->rowCount();
        tableRiders->insertRow(row);
        tableRiders->setItem(row, 0, new QTableWidgetItem(QString::number(rd.id)));
        tableRiders->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(rd.name)));
        tableRiders->setItem(row, 2, new QTableWidgetItem(QString::number(rd.rating, 'f', 2)));
        tableRiders->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(rd.status)));
        tableRiders->setItem(row, 4, new QTableWidgetItem(QString::number(rd.locationId)));
    }

    // 5. Analytics
    AnalyticsData report = system.getAnalyticsReport();
    int totalOrders = system.getOrderCount();
    int completedCount = 0;
    for (const auto &o : system.getOrders())
    {
        if (o.delivered) completedCount++;
    }

    std::ostringstream ss;
    ss << "<h2>System Operational Metrics</h2>"
       << "<p><b>Total Restaurants:</b> " << report.totalRestaurants << "</p>"
       << "<p><b>Total Fleet Riders:</b> " << report.totalRiders << "</p>"
       << "<p><b>Available Riders:</b> " << report.availableRiders << "</p>"
       << "<p><b>Active Pending Orders:</b> " << (totalOrders - completedCount) << "</p>"
       << "<p><b>Completed Deliveries:</b> " << completedCount << "</p>"
       << "<p><b>Average Rider Rating:</b> " << std::fixed << std::setprecision(2) << report.averageRating << " / 5.0</p>";
    lblAnalyticsCards->setText(QString::fromStdString(ss.str()));

    if (mapView)
    {
        mapView->update();
    }
}

void UIManager::onSearchRestaurants()
{
    QString query = editSearchRest->text().trimmed();
    if (query.isEmpty())
    {
        refreshAll();
        return;
    }

    tableRestaurants->setRowCount(0);
    std::vector<std::string> names = system.getSearchManager().getSuggestion(query.toStdString());
    for (const auto &name : names)
    {
        const Restaurant *r = system.getSearchManager().getRestaurantByName(name);
        if (r)
        {
            int row = tableRestaurants->rowCount();
            tableRestaurants->insertRow(row);
            tableRestaurants->setItem(row, 0, new QTableWidgetItem(QString::number(r->id)));
            tableRestaurants->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r->name)));
            tableRestaurants->setItem(row, 2, new QTableWidgetItem(QString::number(r->locationId)));
        }
    }
}

void UIManager::onAddRestaurant()
{
    QString name = editRestName->text().trimmed();
    QString locStr = editRestLoc->text().trimmed();
    if (name.isEmpty() || locStr.isEmpty())
    {
        QMessageBox::warning(this, "Validation", "Please enter restaurant name and valid location ID.");
        return;
    }

    int locId = locStr.toInt();
    if (system.addRestaurant(name.toStdString(), locId))
    {
        QMessageBox::information(this, "Success", "Restaurant added successfully!");
        editRestName->clear();
        editRestLoc->clear();
        refreshAll();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to add restaurant. Check location ID.");
    }
}

void UIManager::onCreateOrder()
{
    int restId = editOrderRestId->text().toInt();
    int custLoc = editOrderCustLoc->text().toInt();
    int prio = editOrderPrio->text().toInt();
    int prepTime = editOrderPrepTime->text().toInt();

    if (prio <= 0 || prepTime <= 0)
    {
        QMessageBox::warning(this, "Validation", "Priority and prep time must be positive numbers.");
        return;
    }

    int orderId = system.createOrder(restId, custLoc, prio, prepTime);
    if (orderId > 0)
    {
        QMessageBox::information(this, "Order Created", "Created Order #" + QString::number(orderId));
        editOrderRestId->clear();
        editOrderCustLoc->clear();
        editOrderPrio->clear();
        editOrderPrepTime->clear();
        refreshAll();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Could not create order. Verify Restaurant ID and Customer Location.");
    }
}

void UIManager::onMarkOrderPrepared()
{
    int orderId = editMarkOrderId->text().toInt();
    if (orderId <= 0) return;

    if (system.markOrderPrepared(orderId))
    {
        QMessageBox::information(this, "Success", "Order #" + QString::number(orderId) + " marked as Prepared.");
        editMarkOrderId->clear();
        refreshAll();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to mark order prepared. Verify kitchen status and order ID.");
    }
}

void UIManager::onCompleteDelivery()
{
    int orderId = editDeliverOrderId->text().toInt();
    if (orderId <= 0) return;

    if (system.completeOrder(orderId))
    {
        QMessageBox::information(this, "Success", "Order #" + QString::number(orderId) + " delivered successfully!");
        editDeliverOrderId->clear();
        refreshAll();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Cannot deliver order. Make sure it is assigned to a rider and prepared.");
    }
}

void UIManager::onSendToStations()
{
    system.getKitchenManager().sendToStations();
    QMessageBox::information(this, "Kitchen", "Dispatched waiting orders from priority heap to stations.");
    refreshAll();
}

void UIManager::onCompleteStationOrder(int station)
{
    KitchenOrder readyOrder;
    if (system.completeKitchenOrder(station, readyOrder))
    {
        QMessageBox::information(this, "Cooking Complete", "Station " + QString::number(station + 1) + " finished Order #" + QString::number(readyOrder.orderId));
        refreshAll();
    }
    else
    {
        QMessageBox::warning(this, "Notice", "Station " + QString::number(station + 1) + " has no active order.");
    }
}

void UIManager::onAddRider()
{
    int id = editRiderId->text().toInt();
    QString name = editRiderName->text().trimmed();
    double rating = editRiderRating->text().toDouble();
    int loc = editRiderLoc->text().toInt();

    if (id <= 0 || name.isEmpty() || rating < 0.0 || rating > 5.0)
    {
        QMessageBox::warning(this, "Validation", "Invalid rider fields. Rating must be 1.0 - 5.0.");
        return;
    }

    Rider r(id, name.toStdString(), rating, "Available", loc);
    if (system.addRider(r))
    {
        QMessageBox::information(this, "Success", "Rider registered/updated.");
        editRiderId->clear();
        editRiderName->clear();
        editRiderRating->clear();
        editRiderLoc->clear();
        refreshAll();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to register rider. Verify location ID.");
    }
}

void UIManager::onAutoAssignRider()
{
    int orderId = editAssignOrderId->text().toInt();
    if (orderId <= 0)
    {
        QMessageBox::warning(this, "Validation", "Please enter a valid Order ID.");
        return;
    }

    if (system.assignRiderToOrder(orderId))
    {
        QMessageBox::information(this, "Success", "Assigned optimal rider to Order #" + QString::number(orderId));
        editAssignOrderId->clear();
        refreshAll();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Could not assign rider. Check if order exists, is prepared, or if riders are available.");
    }
}

void UIManager::onFindRoute()
{
    int src = editRouteSrc->text().toInt();
    int dest = editRouteDest->text().toInt();

    RouteResult res = system.getRouteManager().shortestPath(src, dest);
    if (mapView)
    {
        mapView->setRouteResult(res);
    }

    if (res.routeExists)
    {
        std::ostringstream ss;
        ss << "Shortest Path: ";
        for (size_t i = 0; i < res.path.size(); i++)
        {
            ss << res.path[i];
            if (i + 1 < res.path.size()) ss << " -> ";
        }
        ss << " | Est. Travel Time: " << std::fixed << std::setprecision(1) << res.totalTravelTime << " min";
        lblRouteSummary->setText(QString::fromStdString(ss.str()));
    }
    else
    {
        lblRouteSummary->setText("No route found between Location " + QString::number(src) + " and " + QString::number(dest) + ".");
    }
}
