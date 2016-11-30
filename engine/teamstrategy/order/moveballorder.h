#ifndef MOVEBALLORDER_H_
#define MOVEBALLORDER_H_

#include <order.h>
#include <point_2d.h>

class MoveBallOrder: public Order {
private:
    htwk::point_2d pos;

public:
    MoveBallOrder() = delete;
    MoveBallOrder(const MoveBallOrder&) = delete;
    MoveBallOrder& operator=(MoveBallOrder) = delete;

    MoveBallOrder(float _x,float _y): Order("MoveBallOrder"), pos(htwk::point_2d(_x,_y)){}
    virtual ~MoveBallOrder(){}

    htwk::point_2d getPosition() const{
        return pos;
    }

    virtual void toProtobuf(protobuf::worldmodel::Order* order) {
        order->set_name(getClassName());

        auto* w = order->mutable_moveballorder()->mutable_pos();
        w->set_x(pos.x);
        w->set_y(pos.y);
    }
};

#endif
