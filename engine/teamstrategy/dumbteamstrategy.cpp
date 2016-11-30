#include "dumbteamstrategy.h"

#include <algorithm_ext.h>
#include <fast_math.h>
#include <walktopositionorder.h>
#include <moveballgoalorder.h>

namespace htwk {

void DumbTeamStrategy::ready(const WorldState& worldState, TeamStrategyHelper& helper)
{
    for(const Robot* aliveBot : worldState.aliveRobots)
    {
        helper.setStrategy(aliveBot, new WalkToPositionOrder(robotPos[aliveBot->jerseyNr-1]));
    }
}

void DumbTeamStrategy::play(const WorldState& worldState, TeamStrategyHelper& helper)
{
    const std::vector<Robot*>& aliveRobots = worldState.aliveRobots;

    auto minDistBot = min_element(aliveRobots, [&](const Robot* r1, const Robot* r2) {
        return closer_to(worldState.worldInfo.teamBall, r1->pos, r2->pos);
    });

    for(const Robot* aliveBot : aliveRobots)
    {
        if(aliveBot == *minDistBot) {
            helper.setStrategy(aliveBot, new MoveBallGoalOrder(0));
        } else {
            helper.setStrategy(aliveBot, new WalkToPositionOrder(robotPos[aliveBot->jerseyNr-1]));
        }
    }
}

} // namespace htwk
