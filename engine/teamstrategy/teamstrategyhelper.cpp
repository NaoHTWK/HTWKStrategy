#include "teamstrategyhelper.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <chrono>

#include <fw_constants.h>
#include <orderfactory.h>
#include <nullmq.h>
#include <envinfo.h>

#include <worldmodel.pb.h>

using namespace std::chrono;
namespace wm = protobuf::worldmodel;

TeamStrategyHelper::TeamStrategyHelper(uint8_t ownIdx):
    strategy(NULL), ownIdx(ownIdx),
#ifdef TS_DEBUG
    sockOwnOrder(nullMQ::connect("ownorder" + std::to_string(ownIdx), sysconf(_SC_PAGESIZE)))
#else
    sockOwnOrder(nullMQ::connect("ownorder", sysconf(_SC_PAGESIZE)))
#endif
{

    /*
     * nullMQ has a fixed message size which needs to be know at socket creation time.
     * We have a dynamic message size so we always allocate a memory page for sending.
     * We align it to the beginning of a memory page as this is faster.
     */
    if(posix_memalign(&ownOrderBuf, sysconf(_SC_PAGESIZE), sysconf(_SC_PAGESIZE))) {
        exit(1);
    }

    ownOrderBufSize = sysconf(_SC_PAGESIZE);
}

void TeamStrategyHelper::setStrategy(uint8_t idx, Order *order){
    //	tsLog->debugMsg("deleting %d",idx);
    if(idx>=WM_NUM_PLAYERS){
        exit(-1);
    }
    delete strategy[idx];
    strategy[idx]=order;
}

void TeamStrategyHelper::setStrategy(const Robot* bot, Order *order){
    setStrategy(bot->jerseyNr-1, order);
}

Order *TeamStrategyHelper::getStrategy(uint8_t idx){
    if(idx>=WM_NUM_PLAYERS){
        exit(-1);
    }
    return strategy[idx];
}


void TeamStrategyHelper::initStrategies(){
    strategy=new Order*[WM_NUM_PLAYERS]; //muss in der while-schleife bleiben
    for(int i=0;i<WM_NUM_PLAYERS;i++){
        strategy[i]=new NoOrder(); //Initialisierung
    }
}

#define LENGTH_PREFIX sizeof(size_t)
void TeamStrategyHelper::publishStrategies(){

    static high_resolution_clock::time_point lastTimeNoOrder = high_resolution_clock::now();

    Order* myOrder = strategy[ownIdx];
    NoOrder *noOrder=dynamic_cast<NoOrder*>(myOrder);
    if(noOrder != NULL) {
        high_resolution_clock::time_point curTime = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(curTime - lastTimeNoOrder);
        if(time_span.count() > 1) {
//            tsLog->debugMsg("I've got NO ORDER!!! WAAAAAAHHHHHH!!!!!");
            lastTimeNoOrder = curTime;
        }
    }

    wm::Order wmOrder;
    myOrder->toProtobuf(&wmOrder);

    ((size_t*)ownOrderBuf)[0] = wmOrder.ByteSize();
    if(wmOrder.SerializeToArray(((uint8_t*)ownOrderBuf) + LENGTH_PREFIX, ownOrderBufSize - LENGTH_PREFIX) == false) {}
    sockOwnOrder.send(ownOrderBuf);
}
