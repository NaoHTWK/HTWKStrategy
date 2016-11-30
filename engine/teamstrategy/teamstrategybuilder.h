#ifndef TEAMSTRATEGYBUILDER_H
#define TEAMSTRATEGYBUILDER_H

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>

#include <boost/program_options.hpp>

#include "teamstrategy.h"

class TeamStrategyBuilder
{
private:
    std::map<std::string, std::function<TeamStrategyPtr ()>> strategies;

public:
    TeamStrategyBuilder();

    std::list<std::string> getAvailableStrategies();
    TeamStrategyPtr create(const std::string& name);
    void startAsThread(const std::string& name, int teamNumber, uint8_t ownIdx);
};

#endif // TEAMSTRATEGYBUILDER_H
