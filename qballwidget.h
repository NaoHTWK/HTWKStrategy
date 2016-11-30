#ifndef QBALLWIDGET_H
#define QBALLWIDGET_H

#include <point_2d.h>
#include <QMouseEvent>

#include "qdraggablewidget.h"

class QBallWidget : public QDraggableWidget {
  Q_OBJECT

 public:
  QBallWidget(QWidget *parent = 0);
  htwk::point_2d getPos();

 protected:
  void mousePressEvent(QMouseEvent *event) override;

 private:
  QPointF offset_;
  QPointF pos_;
};

#endif  // QBALLWIDGET_H
