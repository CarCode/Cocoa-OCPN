
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
#ifndef DRAWHELPER_H
#define DRAWHELPER_H
#include <QBrush>
#include <QFont>
#include <QPen>
#include <QPaintEvent>
#include <QPainter>
class DrawHelper
{
public:
    DrawHelper();
    void drawCircles(QPainter *painter, QPaintEvent *event,int metersFromRadar);
private:
    QBrush background;
    QBrush circleBrush;
    QFont textFont;
    QPen circlePen;
    QPen textPen;
};

#endif // DRAWHELPER_H
