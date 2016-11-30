#include "orderfactory.h"

#include <stdio.h>
#include <stdlib.h>

namespace wm = protobuf::worldmodel;

Order* OrderFactory::createOrder(wm::Order& wmOrder)
{
    if(wmOrder.has_moveballorder()) {
        const wm::Point2D& pos = wmOrder.moveballorder().pos();
        Order* order = new MoveBallOrder(pos.x(), pos.y());
        return order;
    }

    if(wmOrder.has_walktopositionorder()) {
        const wm::WalkToPositionOrder& tmpOrder = wmOrder.walktopositionorder();
        const wm::Position& pos = tmpOrder.pos();
        Order* order = new WalkToPositionOrder(pos.x(), pos.y(), pos.a(), tmpOrder.usea());
        return order;
    }

    if(wmOrder.name() == "NoOrder")
        return new NoOrder;

    if(wmOrder.has_moveballgoalorder()){
        const float y = wmOrder.moveballgoalorder().y();
        Order* order = new MoveBallGoalOrder(y);
        return order;
    }

    if(wmOrder.name() == "KeepGoalOrder")
        return new KeepGoalOrder;

    if(wmOrder.name() == "ReceivePassOrder")
        return new ReceivePassOrder;

    fprintf(stderr, "Unknown order. Don't know how to proceed! %s %d\n", __FILE__, __LINE__);
    fprintf(stderr, "Order was: %s\n", wmOrder.name().c_str());
    fflush(stderr);
    exit(1);
}



