#include "strikerBehaviour.h"

#include <array>
#include <cmath>

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <boost/shared_ptr.hpp>

#include <envinfo.h>
#include <fast_math.h>
#include <moveballorder.h>
#include <moveballgoalorder.h>
#include <order.h>
#include <orderfactory.h>

using namespace htwk;
using namespace std;
using namespace boost;
using std::ios_base;
using std::basic_stringbuf;

const float FIELD_SIZE_X = FIELD_LENGTH*0.5f;
const float FIELD_SIZE_Y = FIELD_WIDTH*0.5f;
const float MIN_PASS_DISTANCE = 1.0;
//const float MAX_PASS_DISTANCE = 4.0;
const float MAX_PASS_DISTANCE = 3.0;
const int GOAL_CHANCE_AMOUNT = 9;
const int NO_DIRECT_GOAL_SHOOT = -1;
const point_2d MIDDLEPOINT = point_2d(0,0);
const point_2d goalChance[GOAL_CHANCE_AMOUNT]={ point_2d(FIELD_SIZE_X,0.7),point_2d(FIELD_SIZE_X+0.1,0.6),
                              					point_2d(FIELD_SIZE_X+0.1,0.4),point_2d(FIELD_SIZE_X+0.1,0.2),
				                                point_2d(FIELD_SIZE_X+0.1,0),point_2d(FIELD_SIZE_X+0.1,-0.2),
				                                point_2d(FIELD_SIZE_X+0.1,-0.4),point_2d(FIELD_SIZE_X+0.1,-0.6),
				                                point_2d(FIELD_SIZE_X,-0.7)};
enum class decision { NO_DECISION,
	            SHOOT_GOAL,
				DRIBL_GOAL,
				PASSING,
				DRIBBELING};
static decision lastDecision = decision::NO_DECISION;
static point_2d lastPassPos = point_2d(-10,-10);
static Order *passOrder = new NoOrder();
static point_2d ball;


bool StrikerBehaviour::isInFront(point_2d passPosition){
    if ((ball.x < passPosition.x) || (passPosition.x > MIDDLEPOINT.x)){
        return true;
    }
    return false;
}
/*
bool saveShot(Position newPos, ArrayList<Robot> oppRobots, double saveRange){
    //ball = A ; newPos = B
    double[] AB= {(newPos.x-ball.x),(newPos.y-ball.y)};
    for (const Robot& oppBot : oppRobots){
        if (oppBot.inactiveTimer != ACTIVE_ROBOT){
            continue;
        }
        final double[] AP={(oppBot.getX()-ball.x),(oppBot.getY()-ball.y)};
        final double lambda=(AP[0]*AB[0]+AP[1]*AB[1])/(AB[0]*AB[0]+AB[1]*AB[1]); //Orthogonalprojektion
        if ((lambda > 0) && (lambda < 1)){
            final double[] OF={(ball.x+lambda*AB[0]),(ball.y+lambda*AB[1])};
            final double[] FP={(oppBot.getX()-OF[0]),(oppBot.getY()-OF[1])};
            final double distToPass= Math.sqrt(FP[0]*FP[0] + FP[1]*FP[1])-SPLConstants.BOT_RADIUS;
            if ((saveRange-distToPass) > 0){
                return false;
            }
        }
    }
    return true;
}*/

void StrikerBehaviour::run(const WorldInfo &world, TeamStrategyHelper &helper, const std::array<bool, WM_NUM_PLAYERS> aliveState){
	ball=world.teamBall;
	Robot bot=world.robots[world.ownIdx];
	int i;
	bool shouldPass;
	float bestPassCandidate;
	switch (lastDecision) {
    case decision::SHOOT_GOAL:
        shouldPass = dist(ball,goalChance[0]) > MAX_PASS_DISTANCE * 1.1 && dist(ball,goalChance[8]) > MAX_PASS_DISTANCE * 1.1;
        break;
    case decision::PASSING:
        shouldPass = dist(ball,goalChance[0]) > MAX_PASS_DISTANCE * 0.9 && dist(ball,goalChance[8]) > MAX_PASS_DISTANCE * 0.9;
        break;
    default:
        shouldPass = dist(ball,goalChance[0]) > MAX_PASS_DISTANCE && dist(ball,goalChance[8]) > MAX_PASS_DISTANCE;
        break;
	}
	if (shouldPass){
		/****************** PASS BEHAVIOUR ********************/
    	bool passFlag=false;
    	bestPassCandidate=numeric_limits<float>::infinity();
    	point_2d passPosition = point_2d(0,0);
	    for (i=0;i<WM_NUM_PLAYERS;i++){
            if (!aliveState[i] || (i == world.ownIdx)){continue;}
	        //int saveCounter=numeric_limits<int>::max();
	        //int newCount=0;
	        const point_2d passSpot = point_2d(world.robots[i].pos.x,world.robots[i].pos.y);
	        //const double distPass=dist(ball,passSpot);
	        //if (ball.x+1.2 <= passPosition.x && world.robots[i].ball.precision>=INTERPOLATION){
	        /*if ((newCount=countEnemysInRange(oppRobots,passCandidate))<saveCounter){*/
            if (ball.x+1.7f <= passSpot.x){
	        	const float curXDist=abs(passSpot.x-ball.x-2.5f);
	        	if (bestPassCandidate>curXDist){
	        		bestPassCandidate=curXDist;
	                passPosition=passSpot;
	                passFlag=true;
	                //saveCounter=newCount;
	            }
	            //}
	        }
	    }
	    if (passFlag){
	    	if (dist(passPosition,lastPassPos)<=0.5f){
	    		helper.setStrategy(world.ownIdx,passOrder);//true
	    		return;
	    	}
	    	lastPassPos=passPosition;
	        if (((ball.y+0.3f) >= passPosition.y) && ((ball.y-0.3f)<= passPosition.y)){
	        	delete passOrder;
		        passOrder=new MoveBallOrder(passPosition.x,passPosition.y);
		        helper.setStrategy(world.ownIdx,passOrder);//true
				lastDecision = decision::PASSING;
	            return;
	            /*if (saveShot(ball,new Position(passPosition.x+0.2f,passPosition.y+0.2f,0),oppRobots,0.2f)){
	            	helper.setStrategy(world.ownIdx,new MoveBallOrder(passPosition.x+0.3f,passPosition.y+0.3f,true));
	                return;
	            }else if (saveShot(ball,new Position(passPosition.x+0.2f,passPosition.y-0.2f,0),oppRobots,0.2)){
	            	helper.setStrategy(world.ownIdx,new MoveBallOrder(passPosition.x+0.3f,passPosition.y-0.3f,true));
	                return; 
	            }*/
	            //helper.setStrategy(world.ownIdx,new MoveBallOrder(bot.pos.x+1.f,bot.pos.y));//false
	            //helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[5].y));
	            //lastDecision = decision::DRIBL_GOAL;
	        }else{
	        	delete passOrder;
		        passOrder=new MoveBallOrder(passPosition.x+0.3f,passPosition.y);
		        helper.setStrategy(world.ownIdx,passOrder);//true
		        lastDecision = decision::PASSING;
		        return;
	        }
	    }else{
	        //vorbei dribbeln?
	        //helper.setStrategy(world.ownIdx,new MoveBallOrder(bot.pos.x+1.f,bot.pos.y));//false
	        helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[5].y));
	        lastDecision = decision::DRIBL_GOAL;
	        lastPassPos = point_2d(-10,-10);
	    	return;
	    }
	}else{
	    /************** GOAL SHOOT ***************/
	    lastPassPos = point_2d(-10,-10);
        double minGoalDist=numeric_limits<double>::infinity();
        int savePosGoal=NO_DIRECT_GOAL_SHOOT;
	    for (int i=1;i<GOAL_CHANCE_AMOUNT-1;i++){
	        const double distGoal=dist(ball,goalChance[i]);
	        //if (saveShot(ball,goalChance[i],oppRobots,0.2)){
	            if (minGoalDist > distGoal){
	                minGoalDist=distGoal;
	                savePosGoal=i;
	            }
	        //}
	    }
	    if (savePosGoal > NO_DIRECT_GOAL_SHOOT){
	    	helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[5].y));//goalChance[savePosGoal].y));
	    	lastDecision = decision::SHOOT_GOAL;
	        /*if (minGoalDist<=(MAX_PASS_DISTANCE-MIN_PASS_DISTANCE)&&(minGoalDist>MIN_PASS_DISTANCE)){
	        	//helper.setStrategy(world.ownIdx,new MoveBallOrder(goalChance[savePosGoal].x,goalChance[savePosGoal].y));//true
	        	helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[savePosGoal].y));
	        }else{
	        	//helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[savePosGoal].x,goalChance[savePosGoal].y));//false
	        	helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[savePosGoal].y));
	        }*/
	        return;
	    }else{
	        if ((bot.pos.y>=0)&&(ball.x<(FIELD_SIZE_X-1))){
	        	//helper.setStrategy(world.ownIdx,new MoveBallOrder(bot.pos.x+1.f,bot.pos.y-0.5f));//false
	        	helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[5].y));
	        	lastDecision = decision::DRIBL_GOAL;
	        }else if (ball.x < (FIELD_SIZE_X-1)){
	        	//helper.setStrategy(world.ownIdx,new MoveBallOrder(bot.pos.x+1.f,bot.pos.y+0.5f));//false
	        	helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[5].y));
	        	lastDecision = decision::DRIBL_GOAL;
	        }else{
	            //helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[5].x,goalChance[5].y));//false
	            helper.setStrategy(world.ownIdx,new MoveBallGoalOrder(goalChance[5].y));
	            //TODO in richtung des tors dribbel
	            lastDecision = decision::DRIBL_GOAL;
	        }
	    }
	}
}

StrikerBehaviour::StrikerBehaviour() {	
}
