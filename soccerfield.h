#ifndef SOCCERFIELD_H
#define SOCCERFIELD_H

#include <QPointF>
#include <QSizeF>
constexpr float m2px_factor = 90.f;

constexpr float mToPx(float m) { return m * m2px_factor; }

constexpr float pxToM(float px) { return px / m2px_factor; }

constexpr float ball_size = mToPx(0.15f);
constexpr float robot_size = mToPx(0.2f);

namespace field {

constexpr float length = 9.f;
constexpr float width = 6.f;
constexpr float penalty_area_length = .6f;
constexpr float penalty_area_width = 2.2f;
constexpr float circle_diameter = 1.5f;
constexpr float border = .7f;
constexpr float penalty_spot = 1.3f;
constexpr float line_width = .05f;

}  // namespace field

constexpr float center_x = mToPx(field::border + field::length / 2);
constexpr float center_y = mToPx(field::border + field::width / 2);

inline QPointF mToQPoint(float x, float y) {
  return QPointF(center_x + mToPx(x), center_y + mToPx(y));
}

inline QPointF mToQPoint(const QPointF &p) { return mToQPoint(p.x(), p.y()); }

inline QPointF screenToM(const QPointF &p) {
  return QPointF(pxToM(p.x() - center_x), pxToM(p.y() - center_y));
}

inline QSizeF mToQSize(float x, float y) { return QSizeF(mToPx(x), mToPx(y)); }

#endif  // SOCCERFIELD_H
