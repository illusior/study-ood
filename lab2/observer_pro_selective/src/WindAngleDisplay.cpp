#include <iostream>

#include "../include/WindAngleDisplay.h"

void WindAngleDisplay::Update(const WeatherWindInfo& data, const IObservable<WeatherWindInfo>& updateSource)
{
	std::cout << "Current Wind angle " << data.windInfo.windAngle << '\n'
			  << "----------------" << '\n';
}