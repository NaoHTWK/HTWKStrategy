#ifndef QSOCCERFIELDWIDGET_H
#define QSOCCERFIELDWIDGET_H

#include <map>
#include <tuple>

#include <envinfo.h>
#include <order.h>
#include <QWidget>

class QSoccerFieldWidget : public QWidget {
  Q_OBJECT
  void paintLines(QPainter &p);
  void paintRobotOrders(QPainter &p);

  typedef std::tuple<LocationInfo, htwk::point_2d, OrderPtr> OrderTuple;
  typedef std::map<int, OrderTuple> OrderMap;

  OrderMap orders;



 public:
  explicit QSoccerFieldWidget(QWidget *parent = 0);
  void setOrder(int robot, LocationInfo& robotPos, const htwk::point_2d &ballPos, OrderPtr o);

 protected:
  void paintEvent(QPaintEvent *event) override;

 signals:

 public slots:
};

#endif  // QSOCCERFIELDWIDGET_H
