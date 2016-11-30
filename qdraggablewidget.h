#ifndef QDRAGGABLEWIDGET_H
#define QDRAGGABLEWIDGET_H

#include <QLabel>
#include <QMouseEvent>

class QDraggableWidget : public QLabel {
  Q_OBJECT

 public:
  QDraggableWidget(QWidget *parent = 0);

  void setPos(const QPointF &pos);
  void moveToPos();

 protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

 private:
  QPointF offset_;
  QPointF pos_;

 signals:
  void moved();
};

#endif  // QDRAGGABLEWIDGET_H
