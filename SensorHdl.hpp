#ifndef SENSORMANAGER_HPP_INCLUDED
#define SENSORMANAGER_HPP_INCLUDED

#include <pthread.h>
#include <sys/time.h>
#include "Vector3D.hpp"

#include "ThreadCtrl.hpp"

class Sensor;
class ConfigFile;

class SensorHdl : public ThreadCtrl
{
public:
	SensorHdl(const ConfigFile* cfg);

	/**
	@brief The current position will be the origin
	**/
	void ResetPosition();

	/**
	@brief The sensor will belive the aerodrone is immobile
	@warning Is the device is moving, this will cause the device to keep moving this way
	**/
	void ResetSpeed();

	/**
	@brief get the current position of the device from the origin, using the integrated speed
	@return The position in meters
	**/
	Vector3D<double> GetPosition();

	/**
	@brief get the current angular position of the device from the origin, using the integrated angular speed
	@note Y axis is 0 and X is -PI/2
	@return The position in radians
	**/
	double GetAngularPosition();

	/**
	@brief Get the current device speed, from the integrated acceleration
	@return The speed in meter/second
	**/
	Vector3D<double> GetSpeed();


	/**
	@brief Get the current device angular speed, from the integrated angular acceleration
	@note Y axis is 0 and X is -PI/2
	@return The speed in radians/sec
	**/
	double GetAngularSpeed();


	/**
	@brief Get the current device speed, from the integrated acceleration
	@return The acceleration in meter/second
	@note The gravity acceleration can be retrieved with GetGravityAcceleration()
	**/
	Vector3D<double> GetAcceleration();


	Vector3D<double> GetGravityAcceleration();




private:
	Sensor* m_sen;

	void OnThreadStart();
	void ThreadProcess();

	struct timeval m_dateAccelX, m_dateAccelY, m_dateAccelZ, m_dateGyro;


	struct timespec m_IntegSleepTime;
	int GetElapsedTimeUSSince(struct timeval* begin);



	//In millimeters or degrees
	Vector3D<double> m_fPos;
	double m_fPosRot;

	//In meter.sec-1 or degrees.sec-1
	Vector3D<double> m_fSpeed;
	double m_fSpeedRot;

	//In meter.sec-2
	Vector3D<double> m_fAccel;

	Vector3D<double> m_vGravityAccel;

};

#endif // SENSORMANAGER_HPP_INCLUDED
