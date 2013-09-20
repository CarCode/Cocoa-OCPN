
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

#include "radarcontrol.h"

RadarControl::RadarControl(QObject *parent) :
    QObject(parent)
{
    groupAddress = QHostAddress("236.6.7.10"); // udp channel for controlling radar
    radarRunning = false;
    udpSocket = new QUdpSocket(this);

    udpSocket->joinMulticastGroup(groupAddress);

    qDebug() << "Brought up control channel for radar...";
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(keepRadarAlive()));
}

void RadarControl::processPendingDatagrams()
{

    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        emit notifyDataReceived(datagram);
    }
}

void RadarControl::keepRadarAlive() {
    qDebug() << "keepRadarAlive ...";
    char keepAliveData[] = { 0xA0, 0xC1};
    QByteArray datagram = QByteArray::fromRawData(keepAliveData, sizeof(keepAliveData));
    int nrOfPackets = udpSocket->writeDatagram(datagram.data(), datagram.size(),
                                               groupAddress, 6680);

}

QByteArray RadarControl::getZoomFromMeter(int meter) {
    char zoomData[] = { 0x03, 0xc1,0xf4,0x01,0x00,0x00,0x00,0x00};
    QString hexadecimal;
    hexadecimal.setNum(meter,16);
    QByteArray aMeter = QByteArray::fromHex(hexadecimal.toLocal8Bit());
    zoomData[2] = aMeter[0];
    zoomData[2] = aMeter[1];
    return QByteArray::fromRawData(zoomData, sizeof(zoomData));
}

void  RadarControl::setZoom(int meter) {
    QByteArray datagram = getZoomFromMeter(meter);
    udpSocket->writeDatagram(datagram.data(), datagram.size(),
                             groupAddress, 6680);
}



  void RadarControl::setRainClutter(int level){
       qDebug() << "setRainClutter level " << QString().number(level);
       QByteArray *datagram = new QByteArray(); //getRainClutterFromInt(level);
       datagram->append((char) 0x06);
       datagram->append((char) 0xc1);
       datagram->append((char) 0x04);
       datagram->append((char) 0x00);
       datagram->append((char) 0x00);
       datagram->append((char) 0x00);
       datagram->append((char) 0x00);
       datagram->append((char) 0x00);
       datagram->append((char) 0x00);
       datagram->append((char) 0x00);
       QString hexadecimal;
       hexadecimal.setNum(level,16);
       QByteArray aLevel = QByteArray::fromHex(hexadecimal.toLocal8Bit());
       datagram->append((char) aLevel[0]);
      udpSocket->writeDatagram(datagram->data(), datagram->size(),
                               groupAddress, 6680);
  }

void  RadarControl::toggleRadar() {

    if(!radarRunning) {
         radarRunning = true;
        qDebug() << "Starting radar...";
        char startData[] = { 0x01, 0xc1, 0x01};
        QByteArray datagram = QByteArray::fromRawData(startData, sizeof(startData));
        int nrOfPackets = udpSocket->writeDatagram(datagram.data(), datagram.size(),
                                                   groupAddress, 6680);
        timer->start(2000);
    } else {
        radarRunning = false;
        qDebug() << "Stopping radar...";
        char stopData[] = { 0x00, 0xc1, 0x00};
        QByteArray datagram = QByteArray::fromRawData(stopData, sizeof(stopData));
        int nrOfPackets = udpSocket->writeDatagram(datagram.data(), datagram.size(),
                                                   groupAddress, 6680);

        timer->stop();
    }

}



