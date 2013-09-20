
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
#include "httpdaemon.h"
#include <QTcpSocket>
#include <QStringList>
#include <QDateTime>
#include <QImage>
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <QDebug>
    HttpDaemon::HttpDaemon(quint16 port, QObject* parent )
        : QTcpServer(parent)
    {
        disabled =false;
        listen(QHostAddress::Any, port);
    }

    void HttpDaemon::incomingConnection(int socket)
    {
        if (disabled)
            return;

        QTcpSocket *s = new QTcpSocket(this);
        connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
        connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
        s->setSocketDescriptor(socket);

    }

    void HttpDaemon::pause()
    {
        disabled = true;
    }

    void HttpDaemon::resume()
    {
        disabled = false;
    }

void HttpDaemon::setRadarImg(QImage img) {
  this->radarImg = img;
}

    void HttpDaemon::readClient()
    {
        if (disabled)
            return;

        QTcpSocket* socket = (QTcpSocket*)sender();
        if (socket->canReadLine()) {
            QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
            if (tokens[0] == "GET") {
                  qDebug() << "GET recevied";

                QByteArray ba;
                QBuffer buffer(&ba);
                radarImg.save(&buffer, "PNG");
                socket->write(ba);
                qDebug() << "outputed image";
                socket->close();

                if (socket->state() == QTcpSocket::UnconnectedState) {
                    delete socket;

                }
            }
        }
    }
    void HttpDaemon::discardClient()
    {
        QTcpSocket* socket = (QTcpSocket*)sender();
        socket->deleteLater();


    }



