#ifndef SENSORMANAGER_HPP_INCLUDED
#define SENSORMANAGER_HPP_INCLUDED

#include <pthread.h>
#include <sys/time.h>
#include "Vector3D.hpp"

#include "LivingThread.hpp"

class Sensor;
class ConfigFile;

class SensorHdl : public LivingThread
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
	Vector3D<float> GetPosition();

	/**
	@brief get the current angular position of the device from the origin, using the integrated angular speed
	@note Y axis is 0 and X is -PI/2
	@return The position in radians
	**/
	float GetAngularPosition();

	/**
	@brief Get the current device speed, from the integrated acceleration
	@return The speed in meter/second
	**/
	Vector3D<float> GetSpeed();


	/**
	@brief Get the current device angular speed, from the integrated angular acceleration
	@note Y axis is 0 and X is -PI/2
	@return The speed in radians/sec
	**/
	float GetAngularSpeed();


	/**
	@brief Get the current device speed, from the integrated acceleration
	@return The acceleration in meter/second
	@note The gravity acceleration can be retrieved with GetGravityAcceleration()
	**/
	Vector3D<float> GetAcceleration();


	Vector3D<float> GetGravityAcceleration();




private:
	Sensor* m_sen;

	void OnThreadStart();
	void ThreadProcess();

	struct timeval m_dateAccelX, m_dateAccelY, m_dateAccelZ, m_dateGyro;


	struct timespec m_IntegSleepTime;
	int GetElapsedTimeUSSince(struct timeval* begin);



	//In millimeters or degrees
	Vector3D<float> m_fPos;
	float m_fPosRot;

	//In meter.sec-1 or degrees.sec-1
	Vector3D<float> m_fSpeed;
	float m_fSpeedRot;

	//In meter.sec-2
	Vector3D<float> m_fAccel;

	Vector3D<float> m_vGravityAccel;

};

#endif // SENSORMANAGER_HPP_INCLUDED
