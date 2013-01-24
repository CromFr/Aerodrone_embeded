#ifndef STABCTRL_INCLUDED
#define STABCTRL_INCLUDED

#include "LivingThread.hpp"
#include <iostream>
#include "Vector3D.hpp"

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

    /**
    @brief Tilts the device to move it
    @arg fRotXDeg if=0 the value is not changed, if !=0, the pointed value (in degree) is taken
    @arg fRotYDeg if=0 the value is not changed, if !=0, the pointed value (in degree) is taken
    **/
	void SetAnglularCompensation(float* fRotXDeg, float* fRotYDeg);

	void ChangeRotCompensation(float f){m_fZRotCompensation+=f;}

	void SetTargetSpeed(const Vector3D<float>& v){m_vTargetSpeed = v;}


private:

	void OnThreadStart();
	void ThreadProcess();


	float m_fGlobalMotorSpeed;
	float m_fZRotCompensation;
	float m_fXAngleCompensation;
	float m_fYAngleCompensation;

	Vector3D<float> m_vTargetSpeed;

	float m_fSensibility;
	float m_fRotSensibility;

	struct timespec m_sleepTime;
};

#endif // STABCTRL_INCLUDED
