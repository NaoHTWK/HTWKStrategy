#include "qrobotwidget.h"

#include <cmath>
#include <string>
#include <iostream>

#include "QPainter"
#include "QWheelEvent"

#include "soccerfield.h"

QRobotWidget::QRobotWidget(int jersey_number, QWidget *parent)
    : jersey_number_(jersey_number), QDraggableWidget(parent) {
  setFixedSize(robot_size + 2, robot_size + 2);
  setPos(QPointF(jersey_number - 5.5, ((jersey_number % 3) - 1) * 1.5));
}

LocationInfo QRobotWidget::getPos() const {
  QPointF p = screenToM(pos() + QPointF(size().width(), size().height()) / 2.f);
  return LocationInfo(p.x(), p.y(), rotation_);
}

void QRobotWidget::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::TextAntialiasing);
  QPen pen(Qt::blue);
  pen.setWidthF(1.5f);
  p.setPen(pen);
  p.drawEllipse(1, 1, robot_size, robot_size);
  QPointF center = QPointF(1 + robot_size / 2, 1 + robot_size / 2);
  p.drawLine(center, center + robot_size / 2.f * QPointF(cos(rotation_), -sin(rotation_)));
  QPen jnpen(Qt::black);
  p.setPen(QPen(Qt::black));
  p.setBrush(QBrush(Qt::white));
  QPainterPath text;
  text.addText(QPointF(robot_size / 2 - 4, robot_size / 2 + 7), QFont("Arial", 13, QFont::Bold),
               QString::fromUtf8(std::to_string(jersey_number_).c_str()));
  p.drawPath(text);
}

void QRobotWidget::wheelEvent(QWheelEvent *event) {
  if (event->angleDelta().y() > 0) {
    rotation_ += M_PI / 50.f;
    while (rotation_ > M_PI) {
      rotation_ -= M_PI * 2;
    }
  } else if (event->angleDelta().y() < 0) {
    rotation_ -= M_PI / 50.f;
    while (rotation_ < -M_PI) {
      rotation_ += M_PI * 2;
    }
  }
  std::cout<<"rot: "<<rotation_<<std::endl;
  repaint();
}
