
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

#include "radarobject.h"

RadarObject::RadarObject(QObject *parent) :
    QObject(parent)
{
    maxX = 0;
    maxY =0;
}
void RadarObject::addPoint(int x, int y) {
     listOfPoints.append(qMakePair(x , y ));
}

bool RadarObject::checkIfIntersect(int x, int y) {

    QPair<int,int> c1= qMakePair(x -1, y - 1);
    QPair<int,int> c2= qMakePair(x + 1, y - 1);
    QPair<int,int> c3= qMakePair(x + 1, y );
    QPair<int,int> c4= qMakePair(x + 1, y + 1 );
    QPair<int,int> c5= qMakePair(x , y + 1);
    QPair<int,int> c6= qMakePair(x - 1, y + 1);
    QPair<int,int> c7= qMakePair(x - 1, y);


    if(listOfPoints.contains(c1)) {
        return true;

    } else if (listOfPoints.contains(c2)) {
        return true;
    }else if (listOfPoints.contains(c3)) {
        return true;
    }else if (listOfPoints.contains(c4)) {
        return true;
    }else if (listOfPoints.contains(c5)) {
        return true;
    }else if (listOfPoints.contains(c6)) {
        return true;
    } else if (listOfPoints.contains(c7)) {
        return true;
    } else {
        return false;
    }
}
