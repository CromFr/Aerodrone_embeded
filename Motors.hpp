#ifndef MOTORS_HPP_INCLUDED
#define MOTORS_HPP_INCLUDED

#include <iostream>

#include <sys/time.h>
//#include <unistd.h>
#include "ConfigFile.hpp"

class Motor
{
public:
	Motor(int nPin)
	{
		m_nPin = nPin;
		m_fSpeed = 0;
	}

	/**
	@brief Change the speed of the motor
	@param fSpeed100 Speed to set, out of 100
	**/
	void SetSpeed(float fSpeed100)
	{
		m_fSpeed = fSpeed100;
	}

	/**
	@brief Returns the current speed of the motor
	**/
	float GetSpeed()
	{
		return m_fSpeed;
	}

	int GetPin()
	{
		return m_nPin;
	}

private:
	int m_nPin;
	float m_fSpeed;

};

class Motors
{
public:
	/**
	@brief Motors handler initialisation
	**/
	Motors(const ConfigFile& cfg)
	{
		m_mot[0] = new Motor(cfg.GetValue<int>("PIN_Motor0"));
		m_mot[1] = new Motor(cfg.GetValue<int>("PIN_Motor1"));
		m_mot[2] = new Motor(cfg.GetValue<int>("PIN_Motor2"));
		m_mot[3] = new Motor(cfg.GetValue<int>("PIN_Motor3"));

		m_fMotorMinSpeed = cfg.GetValue<float>("MOT_MinSpeed")/100.f;

		m_fPWMFreq = cfg.GetValue<float>("MOT_PwmFreq");

		m_fProcessPrecision = cfg.GetValue<float>("MOT_PwmPrecision") * 100.0;

		m_bQuitThread = false;


	}


	/**
	@brief Endless loop function to generate the PWM (to be executed in a separated thread)
	**/
	void PWMThread()
	{
		int nDelayPwmUS = (1.f/m_fPWMFreq)*1000000;

		float fTmp;
		struct timeval begin, current;
		struct timespec sleepTime; sleepTime.tv_sec=0; sleepTime.tv_nsec=m_fProcessPrecision*1000.0;
		float fCalculSimplification = (100-m_fMotorMinSpeed)/10000;
		while(!m_bQuitThread)
		{
			//Init timer
			gettimeofday(&begin, NULL);

			//Init pins to LO
			for(short i=0 ; i<4 ; i++)
				m_mot[i]->GetPin();//->SET TO LO


			//Calculating delays for the motors
			int fDelayMotUS[4];
			for(short i=0 ; i<4 ; i++)
			{
				fTmp = m_mot[i]->GetSpeed();
				if(fTmp == 0)
					fDelayMotUS[i] = 0;
				else
				{
					//fDelayMotUS[i] = nDelayPwmUS*(1-((fTmp/100.f)*(100.f-m_fMotorMinSpeed)+35.f)/100.f);
					fDelayMotUS[i] = nDelayPwmUS*(1-fTmp*fCalculSimplification+0.35);
				}
			}

			//Processing pwm
			bool bMot[4] = {true};
			int nElapsedTimeUS;
			do
			{
				gettimeofday(&current, NULL);
				nElapsedTimeUS = current.tv_usec - begin.tv_usec;

				for(short i=0 ; i<4 ; i++)
				{
					if(bMot[i] && nElapsedTimeUS >= fDelayMotUS[i])
					{
						m_mot[i]->GetPin();//->SET TO HI
						bMot[i] = false;
					}
				}
				nanosleep(&sleepTime, NULL);// @note (crom#1#): SegFault?
			}while(nElapsedTimeUS <= nDelayPwmUS);
		}

		for(short i=0 ; i<4 ; i++)
		{
			//->SET PINS TO LOW
		}

	}

	/**
	@brief Enable the thread execution (done when initializing object)
	**/
	void EnableThread()
	{
		m_bQuitThread = false;
	}

	/**
	@brief Stops the thread (quit endless loop)
	**/
	void QuitThread()
	{
		m_bQuitThread = true;
	}

private:
	Motor* m_mot[4];
	float m_fMotorMinSpeed;
	float m_fProcessPrecision;

	float m_fPWMFreq;
	bool m_bQuitThread;






};

void* MotorsThreadWrapper(void* obj)
{
	Motors* mot = reinterpret_cast<Motors*>(obj);
	mot->EnableThread();
	mot->PWMThread();
}

#endif // MOTORS_HPP_INCLUDED
