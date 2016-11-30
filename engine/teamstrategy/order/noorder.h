#ifndef NOORDER_H_
#define NOORDER_H_

#include "order.h"

class NoOrder: public Order {
public:
    NoOrder(const NoOrder&) = delete;
    NoOrder& operator=(NoOrder) = delete;

    NoOrder() : Order("NoOrder") {}
    virtual ~NoOrder() {}
};

#endif /* NOORDER_H_ */
