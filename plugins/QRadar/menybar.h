
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
#ifndef MENYBAR_H
#define MENYBAR_H
#include <QToolButton>
#include <QWidget>

class MenyBar : public QWidget
{
    Q_OBJECT
public:
    explicit MenyBar(QWidget *parent = 0);

signals:
    void sendConnectToRadar();
    void sendChangeZoomToRadar(int);
    void sendChangeRainClutterToRadar(int);

public slots:
    void connectToRadar();
private slots:
    void increaseZoomToRadar();
    void decreaseZoomToRadar();
  void decreaseRainClutter() ;
  void increaseRainClutter();
private:

    int zoomLevels[16];
    int currentZoomLevel;
      int currentRainClutterLevel;
    QToolButton* createButton(QString title);
    QToolButton *connectToRadarButton;
    
};

#endif // MENYBAR_H
