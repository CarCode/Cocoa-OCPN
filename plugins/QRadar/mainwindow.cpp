#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "radarscreen.h"
#include "radarsource.h"
#include "radarcontrol.h"
#include <QtGui>

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

#include "QDebug"
#include "menybar.h"
#include <QPalette>
MainWindow::MainWindow(RadarSource *radarSource) : QWidget()
{
    RadarScreen *openGL = new RadarScreen( this);
    QGridLayout *layout = new QGridLayout;

    layout->addWidget(openGL, 0, 1);

    MenyBar *menu = new MenyBar( this);
    QPalette Pal(palette());
    // set black background
    Pal.setColor(QPalette::Background, Qt::black);
    menu->setAutoFillBackground(true);
    menu->setPalette(Pal);

    layout->addWidget(menu, 0, 0);
    layout->addWidget(openGL, 0, 1);
    setLayout(layout);
    RadarControl *control = new RadarControl;

    QObject::connect(menu, SIGNAL(sendConnectToRadar()),
                     control, SLOT(toggleRadar()));
    QObject::connect(menu, SIGNAL(sendChangeZoomToRadar(int)),
                     control, SLOT(setZoom(int)));
    QObject::connect(menu, SIGNAL(sendChangeRainClutterToRadar(int)),
                     control, SLOT(setRainClutter(int)));

    QObject::connect(radarSource, SIGNAL(notifyDataReceived(QByteArray)),
                     openGL, SLOT(radarDataReceived(QByteArray)));

}

MainWindow::~MainWindow()
{

}
