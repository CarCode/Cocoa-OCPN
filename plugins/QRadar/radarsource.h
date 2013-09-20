
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

#ifndef RADARSOURCE_H
#define RADARSOURCE_H

#include <QObject>
#include <QHostAddress>
#include <QtNetwork>

class QUdpSocket;
class RadarSource : public QObject
{
   Q_OBJECT
public:
    explicit RadarSource(QObject *parent = 0);

private slots:
    void processPendingDatagrams();
signals:
   void notifyDataReceived(QByteArray data);
private:
     QUdpSocket *udpSocket;
      QHostAddress groupAddress;
public slots:
    
};

#endif // RADARSOURCE_H
