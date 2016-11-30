#ifndef TEAMSTRATEGY_H
#define TEAMSTRATEGY_H

#include <stdint.h>

#include <memory>

class TeamStrategy
{
public:
    TeamStrategy();
    virtual ~TeamStrategy();

    virtual void run(int teamNumber, uint8_t ownIdx) = 0;
};

typedef std::shared_ptr<TeamStrategy> TeamStrategyPtr;

#endif // TEAMSTRATEGY_H
