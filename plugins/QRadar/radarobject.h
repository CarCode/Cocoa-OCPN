
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

#ifndef RADAROBJECT_H
#define RADAROBJECT_H
#include <QPair>
#include <QObject>

class RadarObject : public QObject
{
    Q_OBJECT
public:
    explicit RadarObject(QObject *parent = 0);
   int maxX ;
   int maxY ;
   QList<QPair<int, int> > listOfPoints;
   void addPoint(int x, int y);
   bool checkIfIntersect(int x, int y);
signals:
    
public slots:
    
};

#endif // RADAROBJECT_H
