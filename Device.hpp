#ifndef DEVICE_HPP_INCLUDED
#define DEVICE_HPP_INCLUDED

#include <iostream>

class SensorManager;
class Motors;
class StabCtrl;
class NetCtrl;




class Device
{
public:
	Device();
	~Device();


    static Device* GetDevice(){return m_instance;}
    static Motors* GetMotors(){return m_instance->m_mot;}
    static SensorManager* GetSensors(){return m_instance->m_sen;}

	/**
	@brief This is a set of functions to begin moving the drone
	**/
	void StartupRoutine();

	/**
	@brief Land the engine within max 4 seconds and stops the motors
	@note This function will take several seconds to execute
	**/
	void OnErrorLand();

	/**
	@brief Land the engine within max 1 seconds and stops the motors
	@note This function will take several seconds to execute
	**/
	void OnCriticalErrorLand();

private:
    static Device* m_instance;

	Motors* m_mot;
	SensorManager* m_sen;

	StabCtrl* m_stabctrl;
	NetCtrl* m_netctrl;





};

#endif // DEVICE_HPP_INCLUDED
