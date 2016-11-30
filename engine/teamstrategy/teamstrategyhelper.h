#ifndef TEAMSTRATEGYHELPER_H_
#define TEAMSTRATEGYHELPER_H_
#include <iostream>
#include <sstream>
#include <order.h>
#include <stdint.h>
#include <envinfo.h>
#include <nullmq.h>

class TeamStrategyHelper{
private:
    void *ownOrderBuf;
    uint32_t ownOrderBufSize;
    Order **strategy;
    uint8_t ownIdx;
    nullMQ::socket_t sockOwnOrder;

public:
    TeamStrategyHelper(uint8_t ownIdx);
    void initStrategies();
    void publishStrategies();
    void setStrategy(uint8_t idx,Order *order);
    void setStrategy(const Robot *bot, Order *order);
    Order *getStrategy(uint8_t idx);
};

#endif /* TEAMSTRATEGYHELPER_H_ */
