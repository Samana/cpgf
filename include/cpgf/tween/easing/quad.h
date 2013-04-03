#ifndef __TWEEN_EASING_QUAD_H
#define __TWEEN_EASING_QUAD_H

#include "cpgf/tween/gtweeneaseparam.h"

namespace cpgf {

struct QuadEase
{
	static GTweenNumber easeIn(const GTweenEaseParam * param) {
		GTweenNumber t = param->current / param->total;
		return t * t;
	}
	
	static GTweenNumber easeOut(const GTweenEaseParam * param) {
		GTweenNumber t = param->current / param->total;
		return - t * (t - 2);
	}
	
	static GTweenNumber easeInOut(const GTweenEaseParam * param) {
		GTweenNumber t = param->current / (param->total * 0.5);
		if(t < 1) {
			return 0.5 * t * t;
		}
		else {
			--t;
			return -0.5 * (t * (t-2) - 1);
		}
	}
};


} // namespace cpgf


#endif
