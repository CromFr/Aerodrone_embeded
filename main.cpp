#include <iostream>

#include <unistd.h>
#include <signal.h>

#include "Device.hpp"


bool bQuit(false);

void OnSigInt(int signum)
{
    std::cout<<std::endl<<"\e[31m/!\\ ==> Exiting program !!!\e[m"<<std::endl;


    std::clog<<"\e[31mDeleting Device...\e[m"<<std::endl;
    delete Device::GetDevice();
    std::clog<<"--> \e[32mDevice deleted\e[m"<<std::endl;

    exit(signum);
}

int main()
{
	std::clog<<"\e[36m###############################################"<<std::endl;
	std::clog<<"Aerodrone Project by \"Club'Elec ISEN Brest\""<<std::endl;
	std::clog<<"Complete build date: "<<__DATE__<<", "<<__TIME__<<std::endl;
	std::clog<<"###############################################\e[m"<<std::endl<<std::endl;

	#ifndef DEBUG
	if(getuid()!=0)
	{
		std::cerr<<"This program must be run as root in order to command pinout\nDo you want to continue? [y/n]"<<std::endl;
		char c;
		std::cin>>c;
		if(c!='y' && c!='Y')
		{
			std::cerr<<"\e[31mExiting program due to unprivileged execution\e[m"<<std::endl;
			return 255;
		}

	}
	#endif // DEBUG

	signal(SIGINT, OnSigInt);

	Device* dev = new Device;
	//dev->StartupRoutine();

	#ifdef DEBUG
	while(!bQuit)
	{
		std::cout<<"\nMain\n";
		sleep(1);
	}
	#else
	sleep(-1);
	#endif //DEBUG

	return 0;
}
