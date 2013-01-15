#ifndef MOTORS_HPP_INCLUDED
#define MOTORS_HPP_INCLUDED

#include <iostream>
#include <sys/time.h>

#include "ThreadCtrl.hpp"

class ConfigFile;
class Motor;


class MotorHdl : public ThreadCtrl
{

public:
	/**
	@brief Motors handler initialisation
	**/
	MotorHdl(const ConfigFile* cfg);
	~MotorHdl();



	void SetSpeed(int motID, float fPercent);

	float GetSpeed(int motID);

private:
	void ThreadProcess();
	void OnThreadEnd();


	Motor* m_mot[4];


	float m_fMotorMinSpeed;
	float m_fPWMFreq;
	struct timespec m_PWMCheckSleep;








};



#endif // MOTORS_HPP_INCLUDED
