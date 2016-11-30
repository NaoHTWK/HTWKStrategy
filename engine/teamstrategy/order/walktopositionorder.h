#ifndef WALKTOPOSITIONORDER_H_
#define WALKTOPOSITIONORDER_H_

#include <order.h>
#include <position.h>
#include <worldmodel.pb.h>

class WalkToPositionOrder: public Order {
private:
    Position pos;
    bool useA;

public:
    WalkToPositionOrder() = delete;
    WalkToPositionOrder(const WalkToPositionOrder&) = delete;
    WalkToPositionOrder& operator=(WalkToPositionOrder) = delete;

    WalkToPositionOrder(float _x,float _y,float _a,bool _useA=true): Order("WalkToPositionOrder"), pos(_x,_y,_a),useA(_useA){}
    WalkToPositionOrder(Position _pos,bool _useA=true): Order("WalkToPositionOrder"), pos(_pos),useA(_useA){}
    virtual ~WalkToPositionOrder(){}

    Position getPosition() const {
        return pos;
    }

    bool isAlphaRelevant() const{
        return useA;
    }

    virtual void toProtobuf(protobuf::worldmodel::Order* order) {
        order->set_name(getClassName());

        auto* wpos = order->mutable_walktopositionorder()->mutable_pos();

        wpos->set_x(pos.x);
        wpos->set_y(pos.y);
        wpos->set_a(pos.a);

        order->mutable_walktopositionorder()->set_usea(useA);
    }
};

#endif
