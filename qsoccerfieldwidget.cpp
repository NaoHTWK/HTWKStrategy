#include "qsoccerfieldwidget.h"

#include <assert.h>

#include <QBrush>
#include <QLineF>
#include <QPainter>
#include <QPalette>
#include <QPen>

#include <orderfactory.h>

#include "soccerfield.h"

QSoccerFieldWidget::QSoccerFieldWidget(QWidget *parent) : QWidget(parent) {
  QPalette pal(palette());
  pal.setColor(QPalette::Background, Qt::darkGreen);
  setAutoFillBackground(true);
  setPalette(pal);
  setFixedSize(mToPx(field::length + 2 * field::border), mToPx(field::width + 2 * field::border));
}

void QSoccerFieldWidget::setOrder(int robot, LocationInfo &robotPos, const htwk::point_2d &ballPos, OrderPtr o)
{
  orders[robot] = std::make_tuple(robotPos, ballPos, o);
  update();
}

void QSoccerFieldWidget::paintLines(QPainter &p) {
  QPen line_pen(Qt::white);
  line_pen.setWidthF(mToPx(field::line_width));
  line_pen.setCapStyle(Qt::FlatCap);
  line_pen.setJoinStyle(Qt::MiterJoin);
  p.setPen(line_pen);
  p.drawRect(QRectF(mToQPoint(-field::length / 2, -field::width / 2),
                    mToQSize(field::length, field::width)));
  p.drawPolyline(
      QPolygonF(QRectF(mToQPoint(-field::length / 2, -field::penalty_area_width / 2),
                       mToQSize(field::penalty_area_length, field::penalty_area_width))));
  p.drawPolyline(
      QPolygonF(QRectF(mToQPoint(field::length / 2, field::penalty_area_width / 2),
                       mToQSize(-field::penalty_area_length, -field::penalty_area_width))));
  p.drawLines(QVector<QLineF>({
      QLineF(mToQPoint(0, -field::width / 2), mToQPoint(0, field::width / 2)),
      QLineF(mToQPoint(-field::line_width, 0), mToQPoint(field::line_width, 0)),

      QLineF(mToQPoint(-field::length / 2 + field::penalty_spot - field::line_width, 0),
             mToQPoint(-field::length / 2 + field::penalty_spot + field::line_width, 0)),
      QLineF(mToQPoint(-field::length / 2 + field::penalty_spot, -field::line_width),
             mToQPoint(-field::length / 2 + field::penalty_spot, field::line_width)),

      QLineF(mToQPoint(field::length / 2 - field::penalty_spot - field::line_width, 0),
             mToQPoint(field::length / 2 - field::penalty_spot + field::line_width, 0)),
      QLineF(mToQPoint(field::length / 2 - field::penalty_spot, -field::line_width),
             mToQPoint(field::length / 2 - field::penalty_spot, field::line_width)),
  }));
  p.drawEllipse(QRectF(mToQPoint(-field::circle_diameter / 2, -field::circle_diameter / 2),
                       mToQSize(field::circle_diameter, field::circle_diameter)));
}

void QSoccerFieldWidget::paintRobotOrders(QPainter &p) {

  for(const auto& robotOrder : orders)
  {
    LocationInfo pos = std::get<0>(robotOrder.second);
    htwk::point_2d ball = std::get<1>(robotOrder.second);
    OrderPtr order = std::get<2>(robotOrder.second);

    KeepGoalOrder *keepGoalOrder = dynamic_cast<KeepGoalOrder*>(order.get());
    MoveBallOrder *moveBallOrder = dynamic_cast<MoveBallOrder*>(order.get());
    MoveBallGoalOrder *moveBallGoalOrder = dynamic_cast<MoveBallGoalOrder*>(order.get());
    NoOrder *noOrder = dynamic_cast<NoOrder*>(order.get());
    ReceivePassOrder *receiverPassOrder = dynamic_cast<ReceivePassOrder*>(order.get());
    WalkToPositionOrder *walkToPositionOrder = dynamic_cast<WalkToPositionOrder*>(order.get());

    QPen pen(Qt::white);
    pen.setWidthF(mToPx(field::line_width));
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);

    p.setPen(pen);
    p.drawText(mToQPoint(pos.x, pos.y) + QPoint(10, 10), QString::fromStdString(order->getClassName()));

    if(moveBallOrder != NULL) {
      pen.setColor(Qt::yellow);
      p.setPen(pen);
      auto o = moveBallOrder;
      p.drawLine(QLineF(mToQPoint(pos.x, pos.y), mToQPoint(ball.x, ball.y)));
      p.drawLine(QLineF(mToQPoint(ball.x, ball.y), mToQPoint(o->getPosition().x, o->getPosition().y)));
    } else if(walkToPositionOrder) {
      pen.setColor(Qt::magenta);
      p.setPen(pen);
      auto o = walkToPositionOrder;
      p.drawLine(QLineF(mToQPoint(pos.x, pos.y), mToQPoint(o->getPosition().x, o->getPosition().y)));
    } else if(moveBallGoalOrder) {
        pen.setColor(Qt::cyan);
        p.setPen(pen);
        p.drawLine(QLineF(mToQPoint(pos.x, pos.y), mToQPoint(ball.x, ball.y)));
        p.drawLine(QLineF(mToQPoint(ball.x, ball.y), mToQPoint(field::length/2, 0)));
    } else if(keepGoalOrder)  {
        pen.setColor(Qt::magenta);
        p.setPen(pen);
        p.drawLine(QLineF(mToQPoint(pos.x, pos.y), mToQPoint(-field::length/2+.3, 0)));
    } else if(receiverPassOrder || noOrder) {
      /* We don't have anything todo */
    } else {
      /* We don't cover all orders? */
      assert(false);
    }
  }
}

void QSoccerFieldWidget::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  paintLines(p);
  paintRobotOrders(p);
}
