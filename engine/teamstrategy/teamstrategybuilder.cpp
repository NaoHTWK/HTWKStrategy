#include "teamstrategybuilder.h"

#include <stdio.h>
#include <unistd.h>

#include <stdexcept>
#include <thread>

#include "teamstrategyrobocup.h"
#include "dumbteamstrategy.h"

TeamStrategyBuilder::TeamStrategyBuilder()
{
    strategies["robocup"] = []() { return std::make_shared<htwk::TeamStrategyRoboCup>(); };
    strategies["dumb"]    = []() { return std::make_shared<htwk::DumbTeamStrategy>(); };
}

std::list<std::string> TeamStrategyBuilder::getAvailableStrategies()
{
    std::list<std::string> strategiesList;

    for(const auto& s : strategies) {
        strategiesList.push_back(s.first);
    }

    return strategiesList;
}

TeamStrategyPtr TeamStrategyBuilder::create(const std::string& name)
{
    if(strategies.find(name) == strategies.end()) {
        throw(std::invalid_argument("Unknown teamstrategy of name:" + name));
    }

    return strategies[name]();
}

void TeamStrategyBuilder::startAsThread(const std::string& name, int teamNumber, uint8_t ownIdx)
{
    TeamStrategyPtr ts = create(name);

    if(!ts)
    {
        fprintf(stderr, "Error: Teamstrategy is NULL. This should never ever happen %s %d!\n", __FILE__, __LINE__);
        fflush(stderr);
        sleep(1);
        exit(1);
    }

    std::thread tsThread([=]{ ts->run(teamNumber, ownIdx);});
    tsThread.detach();
}
