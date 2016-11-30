
#ifndef TEAMSTRATEGYROBOCUP_H
#define TEAMSTRATEGYROBOCUP_H

#include <vector>
#include <array>

#include <envinfo.h>
#include <position.h>

#include <managedteamstrategy.h>
#include <teamstrategyloader.h>
#include <strikerBehaviour.h>

namespace htwk {

class TeamStrategyRoboCup : public ManagedTeamStrategy {
public:
    TeamStrategyRoboCup();
    virtual ~TeamStrategyRoboCup() {}
    TeamStrategyRoboCup(TeamStrategyRoboCup&) = delete;
    TeamStrategyRoboCup(TeamStrategyRoboCup&&) = delete;

private:
    const int IS_GOALKEEPER = 0;
    const float DEFENSIVE_AT_LINE = -0.75f;
    const float OFFENSIVE_AT_LINE = 1.5f;
    const float FIELD_SIZE_X = FIELD_LENGTH*0.5f;
    const float FIELD_SIZE_Y = FIELD_WIDTH*0.5f;
    const point_2d MIDDLEPOINT = point_2d(0,0);
    const point_2d GOALPOINT = point_2d(4.5,0);

    std::array<bool, WM_NUM_PLAYERS> strikerState;

    naohtwk_timestamp_t lastKickOffTime=0;
    naohtwk_timestamp_t playTime=0;
    naohtwk_timestamp_t noBallTime=0;

    TeamStrategyLoader loader;
    const vector<vector<vector<Position> > > kickoffList = loader.getKickoffList();
    vector<Position> kickoffChoice;
    vector<float> readyMidDist;
    vector<int> readySwitch;

    enum behaviour {KICKOFF, STRIKER, STUPID, GOALY, WALK, NOBALL};
    StrikerBehaviour strikerBehave;
    Position saveKickOffPos=Position(-1.5f,0.f,0.f);
    point_2d lastSeenballPos = point_2d(0,0);

protected:
    void initial(const WorldState& worldState, TeamStrategyHelper& helper) override;
    void ready(const WorldState& worldState, TeamStrategyHelper& helper) override;
    void set(const WorldState& worldState, TeamStrategyHelper& helper) override;
    void play(const WorldState& worldState, TeamStrategyHelper& helper) override;

    void sortPositionSetUp(WorldState worldState, int n[], int start, float distMat[], int min[]);
    void dynamicKickOffPosition(const WorldState &world, const std::vector<Position> &spot, int choseable[], int decision[]);
    void createPositionSetUp(WorldState world, int choseable[], int decision[]);
    htwk::point_2d choseFixSpot(const WorldState &world, const bool playMode);
    void analyseDistance(WorldState world, const float dist[], int sort[]);
    bool decideFixSpot(WorldState world, int atk[], int ownIdx, bool defMode);
    bool findStriker(const WorldState &world, naohtwk_timestamp_t playTime, const htwk::point_2d &ball);
    bool lookforBall(const WorldState &world, const htwk::point_2d &ball);
    float nearNeighbor(const WorldState &world, const Position &simPos);
    float getATKValuation(const WorldState &world, const  Position &simPos, const Robot &curBot, const htwk::point_2d &ball, const bool playMode);
    float getDEFValuation(const WorldState &world, const Position &simPos,const Robot &curBot, const htwk::point_2d &ball, const bool playMode);
    int findKickOffStriker(WorldState world, const std::vector<float> &readyMidDist, const std::vector<int> &readySwitch);
    void prepareKickOffStriker(WorldState world, const std::vector<Position> &kickoffChoice, std::vector<float> &readyMidDist, std::vector<int> &readySwitch, int *posSwitch);

};
}
#endif // TEAMSTRATEGYROBOCUP_H
