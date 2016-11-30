#include "teamstrategyrobocup.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <limits>

#include <fast_math.h>
#include <worldmodelsocket.h>

#include <order.h>
#include <orderfactory.h>
#include <keepgoalorder.h>
#include <moveballorder.h>
#include <moveballgoalorder.h>
#include <noorder.h>

using namespace std;
using std::ios_base;
using std::basic_stringbuf;

namespace htwk {


TeamStrategyRoboCup::TeamStrategyRoboCup() : ManagedTeamStrategy() {
    for (int i=0;i<WM_NUM_PLAYERS;i++)
        strikerState[i]=false;
}

void TeamStrategyRoboCup::initial(const WorldState& worldState, TeamStrategyHelper& helper){
    lastKickOffTime=worldState.curTime;
}

void TeamStrategyRoboCup::ready(const WorldState& worldState, TeamStrategyHelper& helper){
    /* CHOOSE DEF [0] OR ATK [1] KICKOFF POSITIONLIST */
    if (worldState.haveWeKickoff){
        kickoffChoice=kickoffList[1][worldState.aliveCount-1];
    }else{
        kickoffChoice=kickoffList[0][worldState.aliveCount-1];
    }

    /* DYNAMIC POSITION AT KICKOFF */
    int ownPos=0;
    int posSwitch[worldState.aliveCount], choseable[worldState.aliveCount];
    createPositionSetUp(worldState, choseable,posSwitch);
    dynamicKickOffPosition(worldState,kickoffChoice,choseable,posSwitch);
    for (int i=0;i<WM_NUM_PLAYERS;i++){
        if (!worldState.aliveState[i]){
            continue;
        }
        if (i==worldState.worldInfo.ownIdx){
            saveKickOffPos=kickoffChoice[posSwitch[ownPos]];
            helper.setStrategy(i,new WalkToPositionOrder(saveKickOffPos));
        }
        ownPos++;
    }
    prepareKickOffStriker(worldState, kickoffChoice,readyMidDist,readySwitch,posSwitch);

    lastKickOffTime=worldState.curTime;
}

void TeamStrategyRoboCup::set(const WorldState& worldState, TeamStrategyHelper& helper){
    lastKickOffTime=worldState.curTime;
}

void TeamStrategyRoboCup::play(const WorldState& worldState, TeamStrategyHelper& helper){
    const int ownIdx=worldState.worldInfo.ownIdx;
    if(!worldState.aliveState[ownIdx]){
        helper.setStrategy(ownIdx,new NoOrder());
        return;
    }
    const Robot& bot = worldState.worldInfo.robots[ownIdx];
    point_2d ball=worldState.worldInfo.teamBall;
    playTime=(worldState.curTime-lastKickOffTime)/NAO_HTWK_TIMESTAMPS_PER_SEC;

    /* find Behaviour */
    bool isKickOff=(worldState.haveWeKickoff && playTime<5)
            || (!worldState.haveWeKickoff && dist(ball,MIDDLEPOINT)<1 && playTime<9);
    bool iAmStupid=false;
    behaviour botBehaviour = WALK;
    if (ownIdx==IS_GOALKEEPER){botBehaviour=GOALY;}

    if (isKickOff){
        if (ownIdx == findKickOffStriker(worldState, readyMidDist,readySwitch))
            botBehaviour = KICKOFF;
    }else if (worldState.worldInfo.teamBallQuali > 0){
        lastSeenballPos=ball;
        noBallTime=playTime;
        iAmStupid=findStriker(worldState,playTime,ball);
        if (iAmStupid){botBehaviour = STUPID;}
        if (strikerState[ownIdx]){botBehaviour = STRIKER;}
    }else if(ownIdx != IS_GOALKEEPER){
        for (int i=0;i<WM_NUM_PLAYERS;i++)
            strikerState[i]=false;
        botBehaviour = NOBALL;
    }

    switch(botBehaviour){
    case KICKOFF :
        helper.setStrategy(ownIdx,new WalkToPositionOrder(saveKickOffPos));
        break;

    case STRIKER : /*********** Striker Behaviour ************/
        strikerBehave.run(worldState.worldInfo,helper,worldState.aliveState);
        break;

    case STUPID : /*********** StupidStriker Behaviour ************/
        helper.setStrategy(ownIdx,new WalkToPositionOrder(ball.x,ball.y,0,false));
        break;

    case NOBALL : /*********** NoBallSeen Behaviour ************/
        if (lookforBall(worldState,lastSeenballPos))
            helper.setStrategy(ownIdx,new WalkToPositionOrder(lastSeenballPos.x,lastSeenballPos.y,0,false));
        else
            helper.setStrategy(ownIdx,new WalkToPositionOrder(bot.pos.x,bot.pos.y,0,false));
        break;

    case GOALY : /************ Goaly Behaviour ****************/
        helper.setStrategy(ownIdx,new KeepGoalOrder());
        break;

    case WALK :  /************ dynamic positioning ************/
        Position walkToPosition;
        float minValuation=numeric_limits<float>::infinity();
        float curValuation;
        bool playMode = (worldState.haveWeKickoff && ball.x > DEFENSIVE_AT_LINE) || (!worldState.haveWeKickoff && ball.x > OFFENSIVE_AT_LINE);
        const point_2d fixedSpot=choseFixSpot(worldState,playMode); //true = angriff!!
        const point_2d startPoint=point_2d(fixedSpot.x-2.f,fixedSpot.y-2.5f);
        for (float x=startPoint.x;x<startPoint.x+4.f;x+=0.25f){
            for (float y=startPoint.y;y<startPoint.y+5.f;y+=0.25f){
                if (abs(x) > FIELD_SIZE_X || abs(y) > FIELD_SIZE_Y)
                    continue;
                const Position simPos(x,y,0.f);
                if (fixedSpot.x < MIDDLEPOINT.x) {
                    curValuation=getDEFValuation(worldState,simPos,bot,ball,playMode);
                }else{
                    curValuation=getATKValuation(worldState,simPos,bot,ball,playMode);
                }
                if (minValuation > curValuation){
                    minValuation=curValuation;
                    walkToPosition=simPos;
                }
            }
        }

        helper.setStrategy(ownIdx,new WalkToPositionOrder(walkToPosition,false));
    }
}

/*
 * n[]       <- set of robot ids (jersey number -1)
 * start     <- beginning index needed for recursive algorithm
 * distMat[] <- distance from each robot to all possible positions [robot_id][position_id]
 * min[]     -> sorted n[] depending of distMat
 *
 * each robot gets one position (and no robot shares a position)
 * sort all robots to a position, that the overall distances of all robots is min!
 */
void TeamStrategyRoboCup::sortPositionSetUp(WorldState worldState, int n[], int start, float distMat[], int min[]) {
    int i;
    if (start >= worldState.aliveCount) {
        float newDist=0;
        float minDist=0;

        for (i=0;i<worldState.aliveCount;i++){
            newDist+=pow(distMat[i*(worldState.aliveCount+1)+n[i]],2);
            minDist+=pow(distMat[i*(worldState.aliveCount+1)+min[i]],2);
        }

        if (minDist > newDist){
            for (i=0;i<worldState.aliveCount;i++){
                min[i]=n[i];
            }
        }
        return;
    }
    for (i=start; i<worldState.aliveCount;i++) {
        swap(n[start],n[i]);
        sortPositionSetUp(worldState, n,(start+1), distMat, min);
        swap(n[start],n[i]);
    }
}

/*
 * world    <- worldmodel data
 * spot     <- possible positions
 * chosable <- set of robot ids
 * decision -> choses which position a robot gets
 *
 * dynamic position changing in kickOff state
 * -> no order needed in initial state
 */
void TeamStrategyRoboCup::dynamicKickOffPosition(const WorldState &world, const vector<Position> &spot, int choseable[], int decision[]){
    int i,j,k=0;
    float distMat[world.aliveCount*(world.aliveCount+1)];
    for(i=0; i<WM_NUM_PLAYERS; i++){
        if (!world.aliveState[i]){
            continue;
        }
        for (j=0; j<(world.aliveCount+1);j++){
            float dx=spot[j].x-world.worldInfo.robots[i].pos.x;
            float dy=spot[j].y-world.worldInfo.robots[i].pos.y;
            distMat[k*(world.aliveCount+1)+j]=sqrtf(dx*dx+dy*dy);
        }
        k++;
    }
    int activeGoaly=0;
    if (world.aliveState[0])
        activeGoaly=1;
    sortPositionSetUp(world, choseable,activeGoaly,distMat,decision);
}

/*
 * chosable -> set of robot ids from robots who are alive (without goaly)
 * decision -> result, which position a robot gets
 *
 * initialising the set of robot ids, depending on active robots (without goaly)
 */
void TeamStrategyRoboCup::createPositionSetUp(WorldState world, int choseable[], int decision[]){
    int spotIndex=1; //spot for no-goaly strategy
    int activeGoaly=0;
    int i;
    if (world.aliveState[0]){
        choseable[0]=0;
        decision[0]=0;
        activeGoaly=1;
        spotIndex=2; //field-player spots ingame-goaly strategy
    }
    for (i=activeGoaly;i<world.aliveCount;i++){
        choseable[i]=spotIndex;
        decision[i]=spotIndex;
        spotIndex++;
    }
}

/*
 * world    <- worldmodel data
 * playMode <- true: attacking false: defending
 * return   -> atk or def fixSpot
 *
 * start function to decide which robot uses atk or def behaviour
 *  create distance matrix to enemy field border
 */
point_2d TeamStrategyRoboCup::choseFixSpot(const WorldState &world, const bool playMode){
    const point_2d goalyPoint = point_2d(-4.25,0);
    const point_2d fixPointDEF = point_2d(-2,0);
    const point_2d fixPointATK = point_2d(2,0);

    float distMatATK[WM_NUM_PLAYERS];
    for(int i=0; i<WM_NUM_PLAYERS; i++){
        if (!world.aliveState[i]){
            // initialize with possible maximum value
            distMatATK[i]=FIELD_LENGTH;
            continue;
        }
        // distance to enemy field border
        distMatATK[i]=FIELD_SIZE_X-world.worldInfo.robots[i].pos.x;
    }

    if(world.worldInfo.ownIdx == 0 && world.aliveCount>1){
        return goalyPoint;
    }

    int nearATK[WM_NUM_PLAYERS];
    analyseDistance(world, distMatATK, nearATK);

    return decideFixSpot(world, nearATK,world.worldInfo.ownIdx,playMode) ? fixPointATK : fixPointDEF;
}

/*
 * dist[] <- distance of all robots to the enemy field border
 * sort[] -> sorting robot ids 0 .. WM_NUM_PLAYERS
 *
 * sorting distances of all robots
 *  - sort[0] most suitable robot for attacking
 */
void TeamStrategyRoboCup::analyseDistance(WorldState world, const float dist[], int sort[]) {
    int backIndex=WM_NUM_PLAYERS-1;
    int frontIndex=0;

    for(int i=0; i<WM_NUM_PLAYERS; i++){
        if (!world.aliveState[i]){
            sort[backIndex]=-1;
            backIndex--;
            continue;
        }
        int j=0;
        while(j < frontIndex){
            if(dist[sort[j]] > dist[i]){
                for (int k=frontIndex;k>j;k--){
                    sort[k]=sort[k-1];
                }
                break;
            }
            j++;
        }
        sort[j]=i;
        frontIndex++;
    }
}

/*
 * atk[]   <- sorted array of robot ids
 * ownIdx  <- id of the robot, who is executing the algorithm
 * atkMode <- true: attacking false: defensing
 * result  -> robot uses true: attack false: defender behaviour
 *
 * choses how many attacker the current strategy needs
 *  + the behaviour the current running robot
 */
bool TeamStrategyRoboCup::decideFixSpot(WorldState world, int atk[], int ownIdx, bool atkMode){
    const int withoutGoalyAlive = world.aliveState[0] ? (world.aliveCount-1) : world.aliveCount;
    const int divider = withoutGoalyAlive / 2;
    const int num_atk = (withoutGoalyAlive > 2*divider && atkMode) ? divider+1 : divider;

    int i=0;
    while(i < WM_NUM_PLAYERS){
        if (atk[i] == ownIdx)
            break;
        i++;
    }
    if (i < num_atk)
        return true;
    return false;
}

/*
 * world    <- worldmodel data
 * playTime <- time, which pasts in the current play state
 * ball     <- strategy used ball position
 * result   -> behavior desicion of the executing robot true: is STUPID striker false: is not STUPID striker
 *
 * searching for a striker
 * stupidStriker: should be Striker (is near the Ball) but cant see the ball
 */
bool TeamStrategyRoboCup::findStriker(const WorldState &world, naohtwk_timestamp_t playTime, const point_2d &ball){
    ball_precision ballPrecision=INTERPOLATION;
    if (playTime<5)ballPrecision=LONG_INTERPOLATION;

    // get every robot distance to ball (sorted)
    // get lastStriker
    int lastStriker=-1;
    std::map<float,int> distToBall;
    for(int i=0;i<WM_NUM_PLAYERS;i++){
        if (!world.aliveState[i])
            continue;
        distToBall.emplace(std::make_pair(dist(world.worldInfo.robots[i].pos,ball),i));
        if (strikerState[i])
            lastStriker=i;
    }

    // search for striker, it-begin = nearest to ball
    int striker=-1;
    bool iAmStupid = false;
    for(const auto &it : distToBall){
        if(world.worldInfo.robots[it.second].ball.precision >= ballPrecision){
            if(it.second == IS_GOALKEEPER && it.first >= 1.5f && world.aliveCount > 2)
                continue;
            striker = it.second;
            break;
        }
        if(world.worldInfo.ownIdx == it.second)
            iAmStupid=true;
    }

    // does the lastStriker sees the ball? no => reset lastStriker
    if(lastStriker > -1 && world.worldInfo.robots[lastStriker].ball.precision < ballPrecision)
        lastStriker=-1;

    /* Stop striker perm switching
     * new Striker must be an improvement
     * StrikerDist * Penalty
     */
    if (lastStriker > -1 && striker > -1 && lastStriker != striker){
        float strikerDist=dist(world.worldInfo.robots[striker].pos,ball) * 1.1f;
        float lastStrikerDist=dist(world.worldInfo.robots[lastStriker].pos,ball) * (ball.x > world.worldInfo.robots[lastStriker].pos.x ? 0.9f:1.f);

        if (fabs(strikerDist - lastStrikerDist) < 0.5f)
            striker = lastStriker;
    }

    // select striker
    for(int i=0;i<WM_NUM_PLAYERS;i++)
        strikerState[i]=false;
    strikerState[striker]=true;

    return iAmStupid;
}


/*
 * world  <- worldmodel data
 * simPos <- simulated position chosed by the position generator
 * result -> distance of the nearest allied robot
 */
float TeamStrategyRoboCup::nearNeighbor(const WorldState &world, const Position &simPos){
    float minDist=numeric_limits<float>::infinity();
    float distNei=0;
    int i;
    for(i=0; i<WM_NUM_PLAYERS; i++){
        if (!world.aliveState[i] || (i==world.worldInfo.ownIdx)){continue;}
        distNei=dist(simPos,world.worldInfo.robots[i].pos);
        if (minDist>distNei){
            minDist=distNei;
        }
    }
    return minDist;
}

/*
 * world    <- worldmodel data
 * simPos   <- simulated position chosed by the position generator
 * curBot   <- executing robot
 * ball     <- strategy used ball
 * playMode <- true: attacking false: defending
 * result   -> valuation of the simPos as attacker
 *
 * valuation for attacking behaviour (robots in walk mode)
 * all valuations are 1 normalised, but getting different weights in the final valuation
 *
 * 1. borderValuation     - hold min distance to all fieldborder
 * 2. midFieldValuation   - try to stay near the middle of the field (y)
 * 3. minNeiValuation     - hold min distance to nearest allied robot (for passing)
 * 4. maxBallValuation    - try to stay in the range of the ball (makes the attacker more defensive, if the ball is on the own field side)
 * 5. walkValuation       - makes the robot more lazy to walk only small distances (if possible)
 * 6. goalBorderValuation - try to hold a min distance to the enemy goal border
 */
float TeamStrategyRoboCup::getATKValuation(const WorldState &world, const Position &simPos, const Robot &curBot, const htwk::point_2d &ball, const bool playMode){
    const float maxBorderRadius=0.8f;
    const float distToBorder=min(max(0.f,FIELD_SIZE_X-abs(simPos.x)),max(0.f,FIELD_SIZE_Y-abs(simPos.y)));
    const float borderValuation=max(0.f,(maxBorderRadius-distToBorder)/maxBorderRadius); // 1-Norm

    const float distanceToY=1.5f;
    const float midFieldValuation=max(0.f,(abs(simPos.y)-distanceToY)/distanceToY); // 1-Norm

    const float passAVGDistance=3.f;
    const float minNeiValuation=max(0.f,((passAVGDistance-nearNeighbor(world,simPos))/passAVGDistance)); // 1-Norm
    const float maxBallValuation=max(0.f,(dist(ball,simPos)-passAVGDistance)/passAVGDistance); // 1-Norm

    const float walkValuation=dist(simPos,curBot.pos)/FIELD_WIDTH; // more or less 1-Norm
    const float goalAVGDistance=2.5f;
    const float goalBorderValuation=abs((goalAVGDistance-max(0.f,FIELD_SIZE_X-abs(simPos.x)))/goalAVGDistance); // 1-Norm
    int ballWeight=2;
    if (!playMode)
        ballWeight=4;

    return 3*borderValuation + 3*minNeiValuation + ballWeight*maxBallValuation + goalBorderValuation + 2*walkValuation + midFieldValuation;
}

/*
 * world    <- worldmodel data
 * simPos   <- simulated position chosed by the position generator
 * curBot   <- executing robot
 * ball     <- strategy used ball
 * playMode <- true: attacking false: defending
 * result   -> valuation of the simPos as defender
 *
 * valuation for defending behaviour (robots in walk mode)
 * all valuations are 1 normalised, but getting different weights in the final valuation
 *
 * 1. borderValuation  - hold min distance to all fieldborder
 * 2. walkValuation    - makes the robot more lazy to walk only small distances (if possible)
 * 3. ballValuation    - try to stay in the range of the ball (makes the attacker more defensive, if the ball is on the own field side)
 * 4. betweenBallnGoal - tries to stand between the own goal and the ball (all the time)
 * 5. neiValuation     - hold min distance to nearest allied robot (prevent kuddeling)
 * 6. betweenFriends   - try to not run through an allied robot
 */
float TeamStrategyRoboCup::getDEFValuation(const WorldState &world, const Position &simPos, const Robot &curBot, const point_2d &ball, const bool playMode){
    const float maxBorderRadius=0.8f;
    const float distToBorder=min(max(0.f,FIELD_SIZE_X-abs(simPos.x)),max(0.f,FIELD_SIZE_Y-abs(simPos.y)));
    const float borderValuation=max(0.f,(maxBorderRadius-distToBorder)/maxBorderRadius); // 1-Norm
    //distance nearest neighbor
    const float neiValuation=max(0.f,(1.f-nearNeighbor(world,simPos))); //1-Norm
    //walk distance
    const float walkValuation=dist(simPos,curBot.pos)/FIELD_WIDTH; // more or less 1-Norm
    //ball distance
    const float ballValuation=max(0.f,(dist(simPos,ball)-4)/FIELD_WIDTH); //-6 - more or less 1-Norm

    //stop opponents
    point_2d goal=point_2d((-1*(FIELD_SIZE_X+0.2f)),0.f);
    float betweenBallnGoal=0;
    const point_2d AB = point_2d(ball.x-goal.x,ball.y-goal.y);
    const point_2d AP = point_2d(simPos.x-goal.x,simPos.y-goal.y);
    const float lambda=(AP.x*AB.x+AP.y*AB.y)/(AB.x*AB.x+AB.y*AB.y);
    if ((lambda > 0) && (lambda < 1)){
        const point_2d OF=point_2d(goal.x+lambda*AB.x,goal.y+lambda*AB.y);
        const point_2d FP=point_2d(simPos.x-OF.x,simPos.y-OF.y);
        const float distToShot= sqrtf(FP.x*FP.x + FP.y*FP.y);//-0.1; (botSize/2)
        betweenBallnGoal=distToShot/FIELD_WIDTH; // more or less 1-Norm
    }else{
        betweenBallnGoal=10;
    }

    //stop kuddle
    float betweenFriends=0;
    for(int i=0; i<WM_NUM_PLAYERS; i++){
        if ((!world.aliveState[i])||(i==world.worldInfo.ownIdx))
            continue;
        const point_2d AB = point_2d(curBot.pos.x-simPos.x,curBot.pos.y-simPos.y);
        const point_2d AP = point_2d(world.worldInfo.robots[i].pos.x-simPos.x,world.worldInfo.robots[i].pos.y-simPos.y);
        const float lambda=(AP.x*AB.x+AP.y*AB.y)/(AB.x*AB.x+AB.y*AB.y);
        if ((lambda > 0) && (lambda < 1) && dist(curBot.pos,world.worldInfo.robots[i].pos) < 1.5f){
            const point_2d OF=point_2d(simPos.x+lambda*AB.x,simPos.y+lambda*AB.y);
            const point_2d FP=point_2d(world.worldInfo.robots[i].pos.x-OF.x,world.worldInfo.robots[i].pos.y-OF.y);
            const float distDiff= sqrtf(FP.x*FP.x + FP.y*FP.y);
            betweenFriends+=max(0.f,1.f-(distDiff/FIELD_WIDTH)); //1-Norm
        }else{
            betweenFriends+=0;
        }
    }
    int ballWeight=2;
    if (playMode)
        ballWeight=6;

    return 3*borderValuation+2*walkValuation+4*neiValuation+6*betweenBallnGoal+ballWeight*ballValuation+3*betweenFriends;
}

/*
 * world  <- wolrdmodel data
 * ball   <- strategy used ball position
 * reuslt -> true: robot looks for ball false: hold position
 *
 * noBall behaviour
 * decides which robot is going to look for the ball (last seen ball)
 */
bool TeamStrategyRoboCup::lookforBall(const WorldState &world, const point_2d &ball){
    float minDist=numeric_limits<float>::infinity();
    int striker=-1;

    for(int i=0; i<WM_NUM_PLAYERS; i++){
        if (!world.aliveState[i]){continue;}
        float distance=dist(world.worldInfo.robots[i].pos,ball);
        if (minDist > distance){
            minDist=distance;
            striker=i;
        }
    }
    if (world.worldInfo.ownIdx == striker){
        return true;
    }
    return false;
}

/*
 * readyMidDist  <- distances of the kickOff position to the middle point
 * readySwitch   <- holds posSwitch in another collection
 * result        -> choses a robot, who stays striker in the first 10 seconds of play state
 *
 * prevent illegal defender
 * striker shall not change in the first 10 seconds
 *  + expect the striker is not alive anymore
 */
int TeamStrategyRoboCup::findKickOffStriker(WorldState world, const std::vector<float> &readyMidDist, const std::vector<int> &readySwitch){
    float minDist=numeric_limits<float>::infinity();
    int nearestKickOff=-1;
    for(int i=0;i<readyMidDist.size();i++){
        if (minDist > readyMidDist[i]){
            for (int j=0; j < readySwitch.size();j++){
                if (readySwitch[j] == i && world.aliveState[j]){
                    minDist = readyMidDist[i];
                    nearestKickOff=j;
                    break;
                }
            }
        }
    }
    return nearestKickOff;
}

/*
 * kickOffChoice <- set of fix kickOff positions
 * posSwitch     <- set of position ids modified from the dynamic kick off
 * readyMidDist  -> distances of the kickOff position to the middle point
 * readySwitch   -> holds posSwitch in another collection
 */
void TeamStrategyRoboCup::prepareKickOffStriker(WorldState world,
                                                const vector<Position> &kickoffChoice,
                                                std::vector<float> &readyMidDist,
                                                std::vector<int> &readySwitch,
                                                int *posSwitch)
{
    for (const Position& p : kickoffChoice){
        readyMidDist.emplace_back(dist(p, MIDDLEPOINT));
    }
    readySwitch.clear();
    for(int i=0;i<world.aliveCount;i++){
        readySwitch.push_back(posSwitch[i]);
    }
}
} //namespace htwk

