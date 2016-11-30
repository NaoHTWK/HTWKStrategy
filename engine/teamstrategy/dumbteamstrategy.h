#ifndef DUMBTEAMSTRATEGY_H
#define DUMBTEAMSTRATEGY_H

#include <managedteamstrategy.h>
#include <position.h>

namespace htwk {

class DumbTeamStrategy : public ManagedTeamStrategy
{
public:
    DumbTeamStrategy() {};
    virtual ~DumbTeamStrategy() {}
    DumbTeamStrategy(DumbTeamStrategy&) = delete;
    DumbTeamStrategy(DumbTeamStrategy&&) = delete;

protected:
    void ready(const WorldState& worldState, TeamStrategyHelper& helper) override;
    void play(const WorldState& worldState, TeamStrategyHelper& helper) override;

private:
    std::vector<Position> robotPos {
        {-4.2f,   .0f, .0f}, // Goali
        {-1.0f,   .0f, .0f},
        {-3.0f,   .0f, .0f},
        {-2.0f, -2.0f, .0f},
        {-2.0f,  2.0f, .0f}
    };
};

}

#endif // DUMBTEAMSTRATEGY_H
