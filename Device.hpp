#ifndef DEVICE_HPP_INCLUDED
#define DEVICE_HPP_INCLUDED

#include <iostream>
#include <sstream>
#include <pthread.h>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <WiringPi.h>

#include "Motors.hpp"
#include "SensorManager.hpp"


class Device
{
public:
	Device();//Defined in the cpp like
	~Device();//Defined in the cpp like
	static Device* m_instance;
	inline static Device* GetDevice();//Defined in the cpp like


	/**
	@brief This is a set of functions to begin moving the drone
	@note Called at the end of the constructor
	**/
	void StartupRoutine()
	{
        struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 100000000;//every 0.1sec
		for(int nMotorSpeed=0 ; nMotorSpeed<100 ; nMotorSpeed++)
		{
			m_mot->SetSpeed(1, nMotorSpeed);
			m_mot->SetSpeed(2, nMotorSpeed);
			m_mot->SetSpeed(3, nMotorSpeed);
			m_mot->SetSpeed(4, nMotorSpeed);
			nanosleep(&Delay, NULL);
		}
		std::cout<<"Motors at 100% !"<<std::endl;
	}



	/**
	@brief Land the engine within max 4 seconds and stops the motors
	@note This function will take several seconds to execute
	**/
	void OnErrorLand()
	{
		struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 10000000;
		for( ; m_fGlobalMotorSpeed>0 ; m_fGlobalMotorSpeed-=2.5)//Land in 4 seconds max
		{
			ProcessStabilization();
			nanosleep(&Delay, NULL);
		}
		m_mot->QuitThread();
		m_bQuitStabThread = true;
	}


	/**
	@brief Land the engine within max 1 seconds and stops the motors
	@note This function will take several seconds to execute
	**/
	void OnCriticalErrorLand()
	{
		struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 10000000;
		for( ; m_fGlobalMotorSpeed>0 ; m_fGlobalMotorSpeed-=10.0)//Land in 1 seconds max
		{
			ProcessStabilization();
			nanosleep(&Delay, NULL);
		}
		m_mot->QuitThread();
		m_bQuitStabThread = true;
	}

private:

	#include "Device.Stabilization.hpp"
	#include "Device.Network.hpp"


	ConfigFile m_cfg;

	Motors* m_mot;
	SensorManager* m_sen;





};









//====================================================================================
// CPP Like !
Device* Device::m_instance;
Device* Device::GetDevice(){return m_instance;}

Device::Device()
{
	std::cout<<"Initializing Device..."<<std::endl;

	ConfigFile::Error nError = m_cfg.Load("./config.cfg", false);
	if(nError != ConfigFile::no_error)
	{
		std::cerr<<"-> ConfigFile loading error (./config.cfg) : "<<ConfigFile::GetErrorString(nError)<<std::endl;
		return;
	}
	else
		std::cout<<"-> ConfigFile ok"<<std::endl;

	if (wiringPiSetup() == -1)
	{
		std::cerr<<"-> WiringPi setup failed ! ;("<<std::endl;
		return;
	}
	else
		std::cout<<"-> WiringPi ok"<<std::endl;

	//===> No errors during loading
	m_instance = this;


	//Hardware
	m_mot = new Motors(m_cfg);
	m_sen = new SensorManager(m_cfg);

	m_mot->StartThread();
	m_sen->StartThread();

	//Stabilisation
	m_bQuitStabThread=true;
	m_fGlobalMotorSpeed=0;
	m_fZRotCompensation=0;

	stabSleepTime.tv_sec = 0;
	stabSleepTime.tv_nsec = m_cfg.GetValue<float>("STA_SleepTime")*1000.0;
	m_fSensibility = m_cfg.GetValue<float>("STA_Sensibility");
	m_fRotSensibility = m_cfg.GetValue<float>("STA_RotSensibility");

    //Network
    m_bQuitNetworkThread=true;
    m_nSockPort = m_cfg.GetValue<int>("NET_Port");
    StartNetwork();


    //Init end
	StartupRoutine();

	StartStabilization();

}
Device::~Device()
{
	delete m_mot;
	delete m_sen;
}

#endif // DEVICE_HPP_INCLUDED
