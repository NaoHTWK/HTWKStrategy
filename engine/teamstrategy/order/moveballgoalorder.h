#ifndef MOVEBALLGOALORDER_H_
#define MOVEBALLGOALORDER_H_

#include <order.h>

class MoveBallGoalOrder: public Order {
private:
	float y;

public:
    MoveBallGoalOrder() = delete;
    MoveBallGoalOrder(const MoveBallGoalOrder&) = delete;
    MoveBallGoalOrder& operator=(MoveBallGoalOrder) = delete;

    explicit MoveBallGoalOrder(float _y): Order("MoveBallGoalOrder"), y(_y){}
    virtual ~MoveBallGoalOrder(){}

    float getY() const{ return y; }

    virtual void toProtobuf(protobuf::worldmodel::Order* order) {
        order->set_name(getClassName());
        order->mutable_moveballgoalorder()->set_y(y);
    }
};

#endif
