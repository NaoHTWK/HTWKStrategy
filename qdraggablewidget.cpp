#include "qdraggablewidget.h"

#include <iostream>

#include "soccerfield.h"

QDraggableWidget::QDraggableWidget(QWidget *parent) : QLabel(parent) {}

void QDraggableWidget::moveToPos() {
  move((mToQPoint(pos_) - QPointF(size().width(), size().height()) / 2.f).toPoint());
  emit moved();
}

void QDraggableWidget::setPos(const QPointF &pos) {
  pos_ = pos;
  moveToPos();
}

void QDraggableWidget::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    setPos(screenToM(mapToParent(event->pos()) - offset_));
  }
}

void QDraggableWidget::mousePressEvent(QMouseEvent *event) {
  offset_ = mapToParent(event->pos()) - mToQPoint(pos_);
}
