#pragma once
#include "RoutingBridge.h"

#ifdef ROUTING_WITH_QT
#include <QString>
#include <QStringList>

class QtRoutingBridge {
private:
    const RoutingBridge& routingBridge;

public:
    QtRoutingBridge(const RoutingBridge& bridge) : routingBridge(bridge) {
    }

    QString getRouteText(int sourceId, int destinationId) const {
        RouteDisplayData route = routingBridge.findRoute(sourceId, destinationId);
        return QString::fromStdString(route.routeText);
    }

    QStringList getRouteLocations(int sourceId, int destinationId) const {
        RouteDisplayData route = routingBridge.findRoute(sourceId, destinationId);
        QStringList locations;
        for (int i = 0; i < route.locationCount; i++) {
            locations.append(QString::number(route.locationIds[i]));
        }
        return locations;
    }

    double getRouteMinutes(int sourceId, int destinationId) const {
        return routingBridge.getEstimatedMinutes(sourceId, destinationId);
    }
};
#endif
