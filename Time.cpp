//
// Created by Neirokan on 30.04.2020
//

#include "Time.h"
#include <chrono>

namespace Time
{
	namespace
	{
		double _time;
		double _deltaTime;
	}

	double time()
	{
		return _time;
	}

	double deltaTime()
	{
		return _deltaTime;
	}

	void update()
	{
		std::chrono::duration<double> t = std::chrono::system_clock::now().time_since_epoch();
		_deltaTime = t.count() - _time;
		_time = t.count();
	}
}
