#ifndef ORDERFACTORY_H_
#define ORDERFACTORY_H_

#include <keepgoalorder.h>
#include <moveballgoalorder.h>
#include <moveballorder.h>
#include <noorder.h>
#include <order.h>
#include <receivepassorder.h>
#include <walktopositionorder.h>

#include <worldmodel.pb.h>

class OrderFactory {
private:
    OrderFactory() {}

public:
    static Order* createOrder(protobuf::worldmodel::Order& wmOrder);
};

#endif /* ORDERFACTORY_H_ */
