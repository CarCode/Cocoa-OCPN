
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


#include "radarsource.h"

RadarSource::RadarSource(QObject *parent) :
    QObject(parent)
{
    groupAddress = QHostAddress("236.6.7.8");
    //groupAddress = QHostAddress("236.6.7.10");

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(6678, QUdpSocket::ShareAddress);
    udpSocket->joinMulticastGroup(groupAddress);
    udpSocket->setReadBufferSize(8192);
    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(processPendingDatagrams()));
    qDebug() << "Receiving udp traffic started";

}
void RadarSource::processPendingDatagrams()
{

    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        emit notifyDataReceived(datagram);
    }
}
