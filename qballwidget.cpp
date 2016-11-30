#include "qballwidget.h"

#include <iostream>

#include "soccerfield.h"

QBallWidget::QBallWidget(QWidget *parent) : QDraggableWidget(parent) {
  QPixmap ball_pic =
      QPixmap(":/images/ball.png")
          .scaled(ball_size, ball_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  setPixmap(ball_pic);
  setFixedSize(ball_size, ball_size);
  moveToPos();
}

htwk::point_2d QBallWidget::getPos() {
  QPointF p = screenToM(pos() + QPointF(size().width(), size().height()) / 2.f);
  return htwk::point_2d(p.x(), p.y());
}

void QBallWidget::mousePressEvent(QMouseEvent *event) {
  QDraggableWidget::mousePressEvent(event);
  if (event->buttons() & Qt::RightButton) setPos(QPointF(0, 0));
}
