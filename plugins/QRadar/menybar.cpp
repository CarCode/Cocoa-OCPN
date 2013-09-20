
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

#include "menybar.h"
#include <QGridLayout>
#include "QSlider"
#include <QToolBar>
#include <QDebug>


MenyBar::MenyBar(QWidget *parent) :
    QWidget(parent)
{

    currentZoomLevel =0;
    currentRainClutterLevel = 77;
    int initArray[] = {50,75,100,250,500,750,1000,1500,2000,3000,4000,6000,8000,12000,16000,24000};
    memcpy(zoomLevels, initArray, 4 *16);

    QGridLayout *layout = new QGridLayout;

    QToolButton *zoomOut = createButton("Zooma Ut");
    QToolButton *zoomIn = createButton("Zooma In");
    QToolButton *gainPlus = createButton("Gain +");
    QToolButton *gainMinus = createButton("Gain -");
    QToolButton *seaPlus = createButton("SeaClutter +");
    QToolButton *seaMinus = createButton("SeaClutter -");
    QToolButton *rainPlus = createButton("RainClutter +");
    QToolButton *rainMinus = createButton("RainClutter -");
    connectToRadarButton = createButton("Connect");

    connect(connectToRadarButton, SIGNAL(clicked()), this, SLOT(connectToRadar()));
    connect(zoomIn, SIGNAL(clicked()), this, SLOT(decreaseZoomToRadar()));
    connect(zoomOut, SIGNAL(clicked()), this, SLOT(increaseZoomToRadar()));
    connect(rainMinus, SIGNAL(clicked()), this, SLOT(decreaseRainClutter()));
    connect(rainPlus, SIGNAL(clicked()), this, SLOT(increaseRainClutter()));


    layout->setAlignment(Qt::AlignTop);
    layout->setVerticalSpacing(1);
    layout->addWidget(zoomIn, 0, 0);
    layout->addWidget(zoomOut, 1, 0);
    layout->addWidget(gainPlus, 2, 0);
    layout->addWidget(gainMinus, 3, 0);
    layout->addWidget(seaPlus, 4, 0);
    layout->addWidget(seaMinus, 5, 0);
    layout->addWidget(rainPlus, 6, 0);
    layout->addWidget(rainMinus, 7, 0);
    layout->addWidget(connectToRadarButton, 8, 0);
    setLayout(layout);
}

QToolButton* MenyBar::createButton(QString title) {
    QToolButton *genericButton = new QToolButton(this);
    genericButton->setText(title);
    genericButton->setFixedHeight(50);
    genericButton->setFixedWidth(100);
    return genericButton;

}
void MenyBar::connectToRadar() {
    sendConnectToRadar();
}

void MenyBar::increaseZoomToRadar() {
    if(currentZoomLevel < 15)currentZoomLevel++;
    qDebug() << "current zoomlevel" << QString().number(currentZoomLevel);
    qDebug() << "Zoom changed to " << QString().number(zoomLevels[currentZoomLevel]);
    sendChangeZoomToRadar(zoomLevels[currentZoomLevel]);
}

void MenyBar::decreaseZoomToRadar() {
    if(currentZoomLevel > 0)currentZoomLevel--;
    qDebug() << "current zoomlevel" << QString().number(currentZoomLevel);
    qDebug() << "Zoom changed to " << QString().number(zoomLevels[currentZoomLevel]);
    sendChangeZoomToRadar(zoomLevels[currentZoomLevel]);
}

void MenyBar::decreaseRainClutter() {
    if(currentRainClutterLevel < 80)currentRainClutterLevel++;
    qDebug() << "RainClutter changed to " << QString().number(currentRainClutterLevel);
    sendChangeRainClutterToRadar(currentRainClutterLevel);
}

void MenyBar::increaseRainClutter() {
    if(currentRainClutterLevel > 1)currentRainClutterLevel--;
   sendChangeRainClutterToRadar(currentRainClutterLevel);
}
