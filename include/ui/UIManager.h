#pragma once

#include "../system/FoodDeliverySystem.h"
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QPainter>
#include <vector>

class MapViewWidget : public QWidget
{
    Q_OBJECT
private:
    FoodDeliverySystem &system;
    RouteResult lastRouteResult;

public:
    explicit MapViewWidget(FoodDeliverySystem &fds, QWidget *parent = nullptr);
    void setRouteResult(const RouteResult &res);

protected:
    void paintEvent(QPaintEvent *event) override;
};

class UIManager : public QMainWindow
{
    Q_OBJECT

private:
    FoodDeliverySystem &system;

    QTabWidget *tabWidget;

    // Restaurants Tab
    QTableWidget *tableRestaurants;
    QLineEdit *editSearchRest;
    QLineEdit *editRestName;
    QLineEdit *editRestLoc;
    QPushButton *btnAddRest;

    // Orders Tab
    QTableWidget *tableOrders;
    QLineEdit *editOrderRestId;
    QLineEdit *editOrderCustLoc;
    QLineEdit *editOrderPrio;
    QLineEdit *editOrderPrepTime;
    QPushButton *btnCreateOrder;
    QLineEdit *editMarkOrderId;
    QPushButton *btnMarkPrepared;
    QLineEdit *editDeliverOrderId;
    QPushButton *btnCompleteDelivery;

    // Kitchen Tab
    QTableWidget *tableKitchen;
    QPushButton *btnSendToStations;
    QPushButton *btnCompleteStation1;
    QPushButton *btnCompleteStation2;
    QPushButton *btnCompleteStation3;

    // Riders Tab
    QTableWidget *tableRiders;
    QLineEdit *editRiderId;
    QLineEdit *editRiderName;
    QLineEdit *editRiderRating;
    QLineEdit *editRiderLoc;
    QPushButton *btnAddRider;
    QLineEdit *editAssignOrderId;
    QPushButton *btnAutoAssignRider;

    // Map Tab
    MapViewWidget *mapView;
    QLineEdit *editRouteSrc;
    QLineEdit *editRouteDest;
    QPushButton *btnFindRoute;
    QLabel *lblRouteSummary;

    // Analytics Tab
    QLabel *lblAnalyticsCards;

    void setupUi();
    QWidget *createRestaurantTab();
    QWidget *createOrderTab();
    QWidget *createKitchenTab();
    QWidget *createRiderTab();
    QWidget *createMapTab();
    QWidget *createAnalyticsTab();

public slots:
    void refreshAll();
    void onSearchRestaurants();
    void onAddRestaurant();
    void onCreateOrder();
    void onMarkOrderPrepared();
    void onCompleteDelivery();
    void onSendToStations();
    void onCompleteStationOrder(int station);
    void onAddRider();
    void onAutoAssignRider();
    void onFindRoute();

public:
    explicit UIManager(FoodDeliverySystem &fds, QWidget *parent = nullptr);
    ~UIManager() override = default;

    void run();
};

using MainWindow = UIManager;
