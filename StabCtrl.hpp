#ifndef STABCTRL_INCLUDED
#define STABCTRL_INCLUDED

#include "LivingThread.hpp"
#include <iostream>

class Device;
class ConfigFile;

/**
@brief [Controller] Handle the device stabilization
**/
class StabCtrl : public LivingThread
{
public:
    StabCtrl(ConfigFile* cfg);

	/**
	@brief Land the engine within max fMaxSec seconds and stops the motors
	**/
	void LandRoutine(float fMaxSec);

    /**
    @brief Change the motors speed by adding fAdd to the current speed
    **/
	void ChangeMotorSpeed(float fAdd);

    /**
    @brief Change the motors speed
    **/
	void SetGlobalMotorSpeed(float fValue);


private:

	void ThreadProcess();


	float m_fGlobalMotorSpeed;
	float m_fZRotCompensation;

	float m_fSensibility;
	float m_fRotSensibility;

	struct timespec m_sleepTime;
};

#endif // STABCTRL_INCLUDED
