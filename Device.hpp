#ifndef DEVICE_HPP_INCLUDED
#define DEVICE_HPP_INCLUDED

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <WiringPi.h>

#include "Motors.hpp"
#include "SensorManager.hpp"



void* StabilizerThreadWrapper(void* obj);


class Device
{
public:
	Device();//Defined in the cpp like
	~Device();//Defined in the cpp like
	static Device* m_instance;
	inline static Device* GetDevice();//Defined in the cpp like



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
		m_bQuitThread = true;
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
		m_bQuitThread = true;
	}

private:

	#include "Device.Stabilization.hpp"
	//#include "Device.Network.hpp"


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
	m_sen->StartThread();

	//Stabilisation
	m_bQuitThread=true;
	m_fGlobalMotorSpeed=0;
	m_fZRotCompensation=0;

	sleepTime.tv_sec = 0;
	sleepTime.tv_nsec = m_cfg.GetValue<float>("STA_SleepTime")*1000.0;
	m_fSensibility = m_cfg.GetValue<float>("STA_Sensibility");
	m_fRotSensibility = m_cfg.GetValue<float>("STA_RotSensibility");

}
Device::~Device()
{
	delete m_mot;
	delete m_sen;
}

#endif // DEVICE_HPP_INCLUDED
