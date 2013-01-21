#ifndef MOTORS_HPP_INCLUDED
#define MOTORS_HPP_INCLUDED

#include <iostream>
#include <sys/time.h>

#include "LivingThread.hpp"

class ConfigFile;
class Motor;

/**
@brief [Handler] Handles the motors PWM and allows to modify speeds
@note The software PWM is generated in a separate thread
**/
class MotorHdl : public LivingThread
{

public:
	/**
	@brief Motors handler initialisation
	**/
	MotorHdl(const ConfigFile* cfg);
	~MotorHdl();


    /**
    @brief Change the speed of one motor
    @arg motID The ID of the motor (from 1 to 4)
    @arg fPercent The motor speed from 0 to 100
    **/
	void SetSpeed(int motID, float fPercent);

    /**
    @brief Gets the speed of one motor
    @arg motID The ID of the motor (from 1 to 4)
    @return The motor speed from 0 to 100
    **/
	float GetSpeed(int motID);

private:
	virtual void ThreadProcess();
	void OnThreadEnd();


	Motor* m_mot[4];


	float m_fPWMFreq;
	int m_nDelayPwmUS;
	struct timespec m_PWMCheckSleep;








};



#endif // MOTORS_HPP_INCLUDED
