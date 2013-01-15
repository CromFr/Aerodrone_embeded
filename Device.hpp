#ifndef DEVICE_HPP_INCLUDED
#define DEVICE_HPP_INCLUDED


/**
@mainpage Developping for the AeroDrone

@section Routines vs Functions
Les routines sont des fonctions qui prennent un temps important à s'éxécuter.
Elles sont utilisées par exemple pour une faire décoler le drone, ou le faire attérir.
Souvent certaines fonctions du drone sont désactivées lors des routines (ex: les moteurs à la fin de OnErrorRoutine())

@section Controller vs Handler
Un Handler sert de couche d'abstraction pour gérer une partie du fonctionnement du Device, non suceptible de modifier le comportement du drone.

ex: MotorHdl, SensorHdl

Un Controller est un objet pouvant interprêter l'état du drone et modifier son comportement. Il execute des fonctions propres au Device.
En général il accède à différents handler/controller grace au Device
ex: NetCtrl, StabCtrl






**/
//@subsection


#include <iostream>

class SensorHdl;
class MotorHdl;
class StabCtrl;
class NetCtrl;



/**
@brief Main class that contains almost everything
**/
class Device
{
public:
	Device();
	~Device();

    /**
    @brief Get the current Device from anywhere
    **/
    static Device* GetDevice(){return m_instance;}

    /**
    @brief Get the current Motor manager from anywhere
    **/
    static MotorHdl* GetMotors(){return m_instance->m_mot;}

    /**
    @brief Get the current Sensor manager from anywhere
    **/
    static SensorHdl* GetSensors(){return m_instance->m_sen;}

	/**
	@brief [Routine] Test routine that start the motors from 0% to 100% in 10sec
	**/
	void StartupRoutine();

	/**
	@brief [Routine] Land the engine within max 4 seconds and stops the motors
	**/
	void OnErrorRoutine();

	/**
	@brief [Routine] Land the engine within max 1 seconds and stops the motors
	**/
	void OnCriticalErrorRoutine();

private:
    static Device* m_instance;

	MotorHdl* m_mot;
	SensorHdl* m_sen;

	StabCtrl* m_stabctrl;
	NetCtrl* m_netctrl;





};

#endif // DEVICE_HPP_INCLUDED
