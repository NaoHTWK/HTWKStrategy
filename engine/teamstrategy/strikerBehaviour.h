#ifndef STRIKERBEHAVIOUR_H_
#define STRIKERBEHAVIOUR_H_

#include <point_2d.h>
#include <teamstrategyhelper.h>

class StrikerBehaviour{
	public:
		StrikerBehaviour();
        void run(const WorldInfo &world, TeamStrategyHelper &helper, const std::array<bool, WM_NUM_PLAYERS> aliveState);

	private:
        static bool isInFront(htwk::point_2d passPosition);
		

};

#endif /* STRIKERBEHAVIOUR_H_ */
