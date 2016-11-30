#ifndef QROBOTWIDGET_H
#define QROBOTWIDGET_H

#include <envinfo.h>
#include <QMouseEvent>

#include "qdraggablewidget.h"

class QRobotWidget : public QDraggableWidget {
  Q_OBJECT

 public:
  QRobotWidget(int jersey_number, QWidget *parent = 0);
  LocationInfo getPos() const;

 protected:
  void paintEvent(QPaintEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

 private:
  float rotation_ = 0.f;
  int jersey_number_;
};

#endif  // QROBOTWIDGET_H
