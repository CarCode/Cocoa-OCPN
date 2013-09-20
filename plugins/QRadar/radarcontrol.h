
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
#ifndef RADARCONTROL_H
#define RADARCONTROL_H

#include <QObject>
#include <QHostAddress>
#include <QtNetwork>
class QUdpSocket;
class RadarControl: public QObject
{
    Q_OBJECT
public:
    explicit RadarControl(QObject *parent = 0);
public slots:
    void toggleRadar();
    void setZoom(int);
   void setRainClutter(int);
private slots:
    void processPendingDatagrams();
    void keepRadarAlive();
signals:
   void notifyDataReceived(QByteArray data);
private:
     QUdpSocket *udpSocket;
     QHostAddress groupAddress;
     QTimer *timer;
     QByteArray getZoomFromMeter(int meter);
     bool radarRunning;
};

#endif // RADARCONTROL_H
