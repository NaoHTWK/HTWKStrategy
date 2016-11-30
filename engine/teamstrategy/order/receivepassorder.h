#ifndef RECEIVEPASSORDER_H_
#define RECEIVEPASSORDER_H_

#include <order.h>

class ReceivePassOrder: public Order {
public:
    ReceivePassOrder(const ReceivePassOrder&) = delete;
    ReceivePassOrder& operator=(ReceivePassOrder) = delete;

    ReceivePassOrder() : Order("ReceivePassOrder") {}
    virtual ~ReceivePassOrder() {}

};

#endif /* RECEIVEPASSORDER_H_ */
