#include "managedteamstrategy.h"

#include <worldmodelsocket.h>

ManagedTeamStrategy::ManagedTeamStrategy()
{

}

void ManagedTeamStrategy::run(int teamNumber, uint8_t ownIdx)
{
    WorldState worldState;

    nullMQ::socket_t dcmTimeSocket = nullMQ::connect(DCM_HEARTBEAT_NMQ_SOCK_NAME, sizeof(naohtwk_timestamp_t));
    dcmTimeSocket.recv(&(worldState.curTime),false);

#ifdef TS_DEBUG
    nullMQ::socket_t sockWorldModel=nullMQ::connect(
          WorldModelSocket::WORLD_INFO_SOCKET_NAME + std::to_string(ownIdx), sizeof(WorldInfo));
#else
    nullMQ::socket_t sockWorldModel=nullMQ::connect(WorldModelSocket::WORLD_INFO_SOCKET_NAME,sizeof(WorldInfo));
#endif

    sockWorldModel.recv(&(worldState.worldInfo),false);

    nullMQ::socket_t sockGameController=nullMQ::connect(Gamecontroller::GC_INFO_SOCK_NAME,sizeof(GameInfo));
    sockGameController.recv(&(worldState.gameInfo),false);

    TeamStrategyHelper helper(worldState.worldInfo.ownIdx);

    while(true){
        sockWorldModel.recv(&(worldState.worldInfo),false);

        sockGameController.recv(&(worldState.gameInfo),true);
        helper.initStrategies();
        worldState.aliveRobots.clear();

        worldState.haveWeKickoff = (worldState.gameInfo.kickoffTeam == 13);

        /*
         * find active robots (wlan heartbeat && not penalized)
        */
        worldState.aliveCount = 0;
        for(size_t i = 0; i < WM_NUM_PLAYERS; i++){
            if(worldState.worldInfo.robots[i].is_alive && !worldState.gameInfo.isPenalizedOwn[i]){
                worldState.aliveCount++;
                worldState.aliveState[i]=true;
                worldState.aliveRobots.push_back(&(worldState.worldInfo.robots[i]));
            }else{
                worldState.aliveState[i]=false;
            }
        }

        dcmTimeSocket.recv(&worldState.curTime,true);

        switch(worldState.gameInfo.gamestate) {
        case Gamecontroller::INITIAL:
            initial(worldState, helper);
            break;
        case Gamecontroller::READY:
            ready(worldState, helper);
            break;
        case Gamecontroller::SET:
            set(worldState, helper);
            break;
        case Gamecontroller::PLAY:
            play(worldState, helper);
            break;
        case Gamecontroller::FINISHED:
            finished(worldState, helper);
            break;
        }

        helper.publishStrategies();
    }
}
