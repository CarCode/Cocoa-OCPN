
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

#include "radarscreen.h"
#include <QtGui>
#include <math.h>
#include  "httpdaemon.h"
#define PI 3.14159265
RadarScreen::RadarScreen(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    elapsed = 0;
    setFixedSize(1024, 1024);
    setAutoFillBackground(true);
    metersFromRadar = 0;
    image = QImage(1024, 1024, QImage::Format_RGB32);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(redrawRadarScreen()));
    updateFlag = false;
    timer->start(150);
    daemon = new HttpDaemon(8081, NULL);
     daemon->setRadarImg(image);

}
void RadarScreen::redrawRadarScreen()
{
    if(updateFlag) {
        updateFlag = false;
        repaint();
    }
}
void RadarScreen::radarDataReceived(QByteArray data) {
    // qDebug() << "radar data received " << (data[16]&0xff) ;
    int len=data.size();
    int dataOffset = 31;
    int loop = 8;

    while(len -loop >=536 ) {

        int angleraw=(data[loop + 9]&0xff)<<8 | (data[loop+ 8]&0xff);
        float angle=((float)angleraw*360/4096);
        //qDebug() << "angle=" << angle;
        int sstt=(data[loop + 13]&0xff)<<8|(data[loop + 12]&0xff);

        int meter;
        double SQRT2 = qSqrt(2);
        meter=(int)((double)sstt*10/SQRT2);
        metersFromRadar = meter;

        float rad=(float) ( PI*angle ) / 180;

        double radcos=qCos(rad);
        double radsin=qSin(rad);

        int i2=0;
        for(int i=24;i<536;i++) {
            // Get the pixel color
            int pixel=(data[loop + i]&0xff);

            float r=i2;//*scale;
            QColor color = Qt::black;

            float x=(float) (512+radcos*r);

            float y=(float) (512+radsin*r);

            QRgb  value = qRgb(0, pixel, 0);
            image.setPixel(x,y,value);

            i2++;
        }

        loop +=536;
    }
    updateFlag = true;
    //repaint();
}

void RadarScreen::drawRadarLine(QByteArray lineData) {

}
void RadarScreen::detectObjects() {


    for(int x=0;x<512;x++) {
        for(int y=0;y<512;y++) {
            QColor c = QColor::fromRgb (image.pixel(x,y) );
            if (c != Qt::black) {
                 // Found ourself something
                if(listOfObjects.size() > 0 ) {
                    for( int i=0; i< listOfObjects.size();i++) {
                        RadarObject *ro  = listOfObjects.at(i);
                        bool match = ro->checkIfIntersect(x,y);
                        if(match) {
                            ro->addPoint(x,y);
                        } else {
                             listOfObjects.append(ro);
                        }

                    }
                } else {
                    RadarObject *ro = new RadarObject(this);
                    ro->addPoint(x,y);
                    listOfObjects.append(ro);
                }

            }
        }
    }
}

void RadarScreen::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);

    painter.setRenderHint(QPainter::Antialiasing);
    background = QBrush(QColor(64, 32, 64));
    painter.fillRect(event->rect(), background);
    daemon->setRadarImg(image);

    QPixmap pixmap = QPixmap::fromImage(image);
    painter.drawPixmap(0,0,pixmap);
   // detectObjects();
//QString size = QString::number(listOfObjects.size());
 //   qDebug() << size;
    if(metersFromRadar == 0)metersFromRadar =50;
    drawHelper.drawCircles(&painter,event,metersFromRadar);


    painter.end();
}
