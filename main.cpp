#include <pthread.h>

#include "ConfigFile.hpp"
#include "Motors.hpp"
#include "Sensors.hpp"

#include <unistd.h>

int main()
{


	ConfigFile cfg;
	cfg.Load("./config.cfg", false);
	Motors mot(cfg);

	pthread_t th;
	pthread_create(&th, NULL , MotorsThreadWrapper, &mot);


	Sensors s(cfg);

	while(1)usleep(1000);
	return 0;
}
