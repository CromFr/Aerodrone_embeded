#ifndef STABILIZER_HPP_INCLUDED
#define STABILIZER_HPP_INCLUDED

#include <sys/time.h>

#include "ConfigFile.hpp"
#include "Vector3D.hpp"

void* StabilizerThreadWrapper(void* obj);

class Stabilizer
{
public:
	Stabilizer(const ConfigFile& cfg, Motors* mot, Sensors* sen)
	{
		m_mot = mot;
		m_sen = sen;
		m_bQuitThread=true;
		m_fGlobalMotorSpeed=0;
		m_fZRotCompensation=0;

		sleepTime.tv_sec = 0;
		sleepTime.tv_nsec = cfg.GetValue<int>("STA_SleepTime")*1000;

		m_fSensibility = cfg.GetValue<float>("STA_Sensibility");
		m_fRotSensibility = cfg.GetValue<float>("STA_RotSensibility");

	}

	/**
	@brief Starts the motors and stabilization system
	**/
	void StartStabilization()
	{
		if(m_bQuitThread)
		{
			m_bQuitThread=false;
			pthread_create(&m_thread, NULL , StabilizerThreadWrapper, this);

			m_mot->StartThread();
		}
		else
		{
			std::cerr<<__FILE__<<" @ "<<__LINE__<<" : The thread is already running"<<std::endl;
		}
	}

	/**
	@warning Do not call this function!
	**/
	void StabThread()
	{
		while(!m_bQuitThread)
		{
			std::cout<<"Stab";
			ProcessStab();
			nanosleep(&sleepTime, NULL);
		}
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
			ProcessStab();
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
			ProcessStab();
			nanosleep(&Delay, NULL);
		}
		m_mot->QuitThread();
		m_bQuitThread = true;
	}


private:
	Motors* m_mot;
	Sensors* m_sen;

	float m_fGlobalMotorSpeed;
	float m_fZRotCompensation;

	float m_fSensibility;
	float m_fRotSensibility;

	struct timespec sleepTime;

	bool m_bQuitThread;
	pthread_t m_thread;

	/**
	@brief Equilibrate the device by compensating the motors
	**/
	void ProcessStab()
	{
		float fAccX = m_sen->GetAcceleroX();
		float fAccY = m_sen->GetAcceleroX();
		float fAccZ = m_sen->GetAcceleroX();//@TODO change to x, y z when implemented
		float fRot = m_sen->GetAcceleroX();//@TODO change to rotation

		Vector3D<float> vInclinaison(fAccX, fAccY, fAccZ);
		vInclinaison.Normalize();

		m_fZRotCompensation+=m_fRotSensibility*fRot; //@note may need some better calculus ;)

		float fSpeed[4] = {
							m_fGlobalMotorSpeed + m_fZRotCompensation/2.f + vInclinaison.PlanGetZAt(28.25, 28.25),
							m_fGlobalMotorSpeed + m_fZRotCompensation/2.f + vInclinaison.PlanGetZAt(28.25, -28.25),
							m_fGlobalMotorSpeed + m_fZRotCompensation/2.f + vInclinaison.PlanGetZAt(-28.25, -28.25),
							m_fGlobalMotorSpeed + m_fZRotCompensation/2.f + vInclinaison.PlanGetZAt(-28.25, 28.25)
						};


		//Makes impossible to have speeds under 0 or over 100
		for(int i=0 ; i<4 ; i++)
		{
			if(fSpeed[i]>100.0)
			{
				float fDiff = fSpeed[i]-100.0;
				for(int j=0 ; j<4 ; j++)
					fSpeed[j] -= fDiff;
			}
			else
			{
				float fDiff = -fSpeed[i];
				for(int j=0 ; j<4 ; j++)
					fSpeed[j] += fDiff;
			}
		}

		//Set the speed
		for(int i=0 ; i<4 ; i++)
		{
			m_mot->SetSpeed(i+1, fSpeed[i]);
		}
	}


};

void* StabilizerThreadWrapper(void* obj)
{
	Stabilizer* stab = reinterpret_cast<Stabilizer*>(obj);
	stab->StabThread();
	return 0;
}

#endif // STABILIZER_HPP_INCLUDED
