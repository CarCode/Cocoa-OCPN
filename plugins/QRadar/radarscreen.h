
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

#ifndef RADARSCREEN_H
#define RADARSCREEN_H
#include <QGLWidget>
#include "drawhelper.h"
#include "radarobject.h"
#include  "httpdaemon.h"
class RadarScreen : public QGLWidget
{ Q_OBJECT



public slots:
    void redrawRadarScreen();
    void radarDataReceived(QByteArray data);

protected:
    void paintEvent(QPaintEvent *event);

private:
    //Helper *helper;
    int elapsed;
    QBrush background;
    DrawHelper drawHelper;
    void drawRadarLine(QByteArray lineData);
    QImage image;
    bool updateFlag;
    int metersFromRadar;
    void detectObjects();
     QList<RadarObject*> listOfObjects;
      HttpDaemon *daemon;
public:
    RadarScreen(QWidget *parent);

};

#endif // RADARSCREEN_H
