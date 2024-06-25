#include "qgraphics_arc_item.hpp"

#include <cmath>

#include <QDebug>
#include <QPainter>
#include <qmath.h>

//Function to copy the values necessary for drawing an arc.
QGraphicsArcItem::QGraphicsArcItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent) : QGraphicsEllipseItem(x - width, y - width, 2 * width, 2 * width, parent) 
{ 
    centerPoint = QPointF(x, y);
    startPoint = QPointF(x + width, y);
    spanAngle = height; 
}

// Paint funcction to draw the arc
void QGraphicsArcItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) 
{
    qreal radius = QLineF(centerPoint, startPoint).length();
    QRectF rectangle(centerPoint.x() - radius, centerPoint.y() - radius, 2 * radius, 2 * radius);

    int startAngle = 0;
    spanAngle = spanAngle * (180 / M_PI) * 16; // Convert radians to 1/16th degrees

    QPen pen(Qt::black);  
    painter->setPen(pen);

    painter->drawArc(rectangle, startAngle, spanAngle); 
}

