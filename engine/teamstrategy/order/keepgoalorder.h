#ifndef KEEPGOALORDER_H_
#define KEEPGOALORDER_H_

#include <order.h>

class KeepGoalOrder: public Order {
public:
    KeepGoalOrder(const KeepGoalOrder&) = delete;
    KeepGoalOrder& operator=(KeepGoalOrder) = delete;

    KeepGoalOrder() : Order("KeepGoalOrder") {}
    virtual ~KeepGoalOrder() {}
};

#endif /* KEEPGOALORDER_H_ */
