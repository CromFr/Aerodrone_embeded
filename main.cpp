#include <pthread.h>

#include "ConfigFile.hpp"
#include "Motors.hpp"
#include "Sensors.hpp"
#include "Stabilizer.hpp"

#include <unistd.h>
//
//#include <boost/bind.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/asio.hpp>


int main()
{
	std::cout<<"Program starts !"<<std::endl;

	ConfigFile cfg;
	cfg.Load("./config.cfg", false);


	Motors mot(cfg);
	Sensors sen(cfg);
	Stabilizer stab(cfg, &mot, &sen);
	stab.StartStabilization();

	bool bQuit(false);
	while(!bQuit)
	{
		std::cout<<"\nMain\n";
		struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 500000000;
		nanosleep(&Delay, NULL);
	}
	return 0;
}
