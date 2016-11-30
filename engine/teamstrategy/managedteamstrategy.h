#ifndef MANAGEDTEAMSTRATEGY_H
#define MANAGEDTEAMSTRATEGY_H

#include <teamstrategy.h>

#include <array>
#include <tuple>

#include <envinfo.h>
#include <fw_constants.h>
#include <teamstrategyhelper.h>

class ManagedTeamStrategy : public TeamStrategy
{
protected:
    struct WorldState
    {
        uint8_t aliveCount;
        std::array<bool, WM_NUM_PLAYERS> aliveState;
        std::vector<Robot*> aliveRobots;

        WorldInfo worldInfo;
        GameInfo gameInfo;

        naohtwk_timestamp_t curTime;

        bool haveWeKickoff;

        WorldState() {}
    };

public:
    ManagedTeamStrategy();
    virtual ~ManagedTeamStrategy() {}

    void run(int teamNumber, uint8_t ownIdx) override final;

protected:
    virtual void initial(const WorldState& worldState, TeamStrategyHelper& helper) {}
    virtual void ready(const WorldState& worldState, TeamStrategyHelper& helper) {}
    virtual void set(const WorldState& worldState, TeamStrategyHelper& helper) {}
    virtual void play(const WorldState& worldState, TeamStrategyHelper& helper) {}
    virtual void finished(const WorldState& worldState, TeamStrategyHelper& helper) {}
};

#endif // MANAGEDTEAMSTRATEGY_H
