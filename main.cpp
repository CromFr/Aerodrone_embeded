//#include <pthread.h>
//
//#include "ConfigFile.hpp"
//#include "Motors.hpp"
//#include "Sensors.hpp"
//#include "SensorManager.hpp"
//
//
//#include <unistd.h>

#include <iostream>

#include "Device.hpp"

int main()
{
	std::cout<<"Program starts !"<<std::endl;

	#ifndef TRG_DEBUG
	if(getuid()!=0)
	{
		std::cerr<<"This program must be run as root in order to command pinout\nDo you want to continue? [y/n]"<<std::endl;
		char c;
		std::cin>>c;
		if(c!='y' && c!='Y')
		{
			std::cerr<<"Exiting program due to unprivileged execution"<<std::endl;
			return 255;
		}

	}
	#endif // TRG_DEBUG


	Device dev;

	bool bQuit(false);
	while(!bQuit)
	{
		std::cout<<"\nMain\n";
		struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 500000000;
		nanosleep(&Delay, NULL);
	}
	return 0;
}
