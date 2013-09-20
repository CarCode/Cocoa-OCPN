
/**This file is part of QRadar.

QRadar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

QRadar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with QRadar.  If not, see <http://www.gnu.org/licenses/>.

Copyright 2012  Thomas Söderberg

**/

#include "drawhelper.h"

DrawHelper::DrawHelper()
{
}
void DrawHelper::drawCircles(QPainter *painter, QPaintEvent *event,int metersFromRadar)
{
    qreal r = 1000/1000.0;
    int n = 30;
    int i=1;
    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::black);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));
    QBrush circleBrush = QBrush(Qt::transparent,Qt::SolidPattern);

    circlePen.setWidth(1);
    painter->setBrush(circleBrush);
     QPen circlePen = QPen(Qt::darkGray);
    painter->setPen(circlePen);
    QPoint point = QPoint( 10, 30 );
    QFont *font = new QFont();
    font->setPixelSize(25);
    font->setBold(true);

    painter->setFont(*font);
    painter->drawText( point, QString::number(metersFromRadar) + " meter");
     circlePen = QPen(Qt::darkYellow);
   painter->setPen(circlePen);
       point = QPoint( 10, 60 );
   painter->drawText( point, "25 meter");

    // Screen middle point
    int middlex=1024/2;
    int middley=1024/2;

    qreal radius = 0 + 120.0*((i+r)/n);
    qreal circleRadius = 1 + ((i+r)/n)*300;

    qreal rx = (middlex / metersFromRadar) *25;
    qreal ry =  (middley / metersFromRadar) *25;

    painter->drawEllipse(QPointF(middlex  ,middley),rx,ry);
    painter->drawEllipse(QPointF(middlex  ,middley),rx *2,ry *2);
    circlePen = QPen(Qt::darkGray);
     painter->setPen(circlePen);
    painter->drawEllipse(QPointF(middlex  ,middley),rx *3,ry *3);

}
