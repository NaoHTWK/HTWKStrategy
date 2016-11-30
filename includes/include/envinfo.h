#ifndef ENVINFO_H
#define ENVINFO_H

#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <dcm.h>
#include <fw_constants.h>
#include <gamestatetypes.h>
#include <gamecontroller_client.h>
#include "../vision_primitives/point_2d.h"

struct LocationInfo{
    float x;//!<-x=ownGoal, +x=oppGoal
    float y;//!<positive towards right (at a=0)
    float a;//!<0 rad=towards opp goal, positive towards left
    float qual;//!<1=good, 0=no clue, values inbetween have to be determined experimentally ;-)

    LocationInfo() {}
    LocationInfo(float x, float y, float a, float qual = 1) : x(x), y(y), a(a), qual(qual) {}
};

enum ball_precision{
    NONE=0,                //no Ball found
    LONG_INTERPOLATION,    //ball position estimated from one of several previous frames
    INTERPOLATION,         //ball position estimated from last frame
    NORMAL,                //ball found in camera image and projected on ground
    FOOT_INTERPOLATION,    //ball found in camera image and estimated feet position used for accurate position estimation
    FOOT_MEASUREMENT       //ball found in camera image and projected with accurate feet position detection
};

struct RelativeBall {
    ball_precision precision;//!<precision measure to differentiate quality for near balls
};

struct Robot{
    bool is_alive;//<!True if a message via world model broadcaster was received during in the recent time. A robot can be assumed to me missing if this is false.
    struct RelativeBall ball;//<!The best ball position known to the robot.
    LocationInfo pos;//!<absolute position of the robot, matched for best fit in world model

    uint8_t jerseyNr;
};

struct WorldInfo{
    uint8_t ownIdx; //!<The robot in WorldInfo.robots[ownIdx] is the this WorldInfo was created on.
    Robot robots[WM_NUM_PLAYERS]; //!<Array of all known robots with their most recent information
    float teamBallQuali;//!<BEST_BALL_QUALI..WORST_BALL_QUALI inclusive
    htwk::point_2d teamBall;//!<absolute position of the ball, best guess
};

struct GameInfo {
    bool isPenalizedOwn[WM_NUM_PLAYERS];
    enum Gamecontroller::GameState gamestate;
    uint8_t kickoffTeam;           //!< Team Number of the team that has kickoff
};

#endif// END_INFO_H
