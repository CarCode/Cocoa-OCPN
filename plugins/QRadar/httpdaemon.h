
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

#ifndef HTTPDAEMON_H
#define HTTPDAEMON_H

#include <QObject>
#include <QTcpServer>
#include <QImage>

class HttpDaemon : public QTcpServer
{
    Q_OBJECT
public:
    HttpDaemon(quint16 port, QObject* parent = 0) ;

    void incomingConnection(int socket);

    void pause();

    void resume();
    void setRadarImg(QImage img);

private slots:
    void readClient();
    void discardClient();
private:
    bool disabled;
    QImage radarImg;

};


#endif // HTTPDAEMON_H
