#ifndef __FAST_MATH_H__
#define __FAST_MATH_H__

#include <list>
#include <string>
#include <vector>

#include <stdint.h>
#include <math.h>
#include <stdio.h>

// clamps the values to [-PI, PI]
template <typename T> T normalizeRotation(T val) {
	while (val > M_PI )
		val -= M_PI * 2.f;
	while (val < -M_PI )
		val += M_PI * 2.f;
	return val;
}

template<typename T1,typename T2>
static float dist(const T1 &pos1,const T2 &pos2){
	return sqrtf((pos1.x-pos2.x)*(pos1.x-pos2.x)+(pos1.y-pos2.y)*(pos1.y-pos2.y));
}

template<class U, class T>
bool closer_to(const U& what, const T& c1, const T& c2) { return dist(c1, what) < dist(c2, what); };

#endif
