#ifndef SENSORMANAGER_HPP_INCLUDED
#define SENSORMANAGER_HPP_INCLUDED

#include "Sensors.hpp"

#include "timetools.h"
#include "Vector3D.hpp"


class SensorManager
{
public:
	SensorManager(const ConfigFile& cfg) : m_sen(cfg), m_fPos(0,0,0), m_fSpeed(0,0,0), m_fAccel(0,0,0)
	{
		ResetPosition();
		ResetSpeed();
		m_fGravityAccel=0;
	}

	/**
	@brief records the gravity acceleration cst, and
	@warning Must be called when the device is not moving(landed)
	**/
	void RecordGravityAccel()
	{
		Vector3D<double> accel(m_sen.GetAcceleroX(), m_sen.GetAcceleroY(), m_sen.GetAcceleroZ());
		m_fGravityAccel = accel.GetLength();
	}

	void ResetPosition()
	{
		m_fPos.Set(0,0,0);
		m_fPosRot=0;
	}

	void ResetSpeed()
	{
		m_fSpeed.Set(0,0,0);
		m_fSpeedRot=0;
	}


	Vector3D<double> GetPosition()
	{
		return m_fPos;
	}

	double GetAngularPosition()
	{
		return m_fPosRot;
	}

	Vector3D<double> GetSpeed()
	{
		return m_fSpeed;
	}

	double GetAngularSpeed()
	{
		return m_fPosRot;
	}


	Vector3D<double> GetAcceleration()
	{
		return m_fAccel;
	}

	void Process()
	{
		struct timeval accelX, accelY, accelZ, gyro;
		gettimeofday(&accelX, NULL);
		accelY = accelX; accelZ = accelX; gyro = accelX;
		while(1)
		{
			m_fAccel.x = m_sen.GetAcceleroX();
			int nElapsedAX = GetElapsedTimeUSSince(&accelX);
			gettimeofday(&accelX, NULL);

			m_fAccel.y = m_sen.GetAcceleroY();
			int nElapsedAY = GetElapsedTimeUSSince(&accelY);
			gettimeofday(&accelY, NULL);

			m_fAccel.z = m_sen.GetAcceleroZ();
			int nElapsedAZ = GetElapsedTimeUSSince(&accelZ);
			gettimeofday(&accelZ, NULL);

			m_fSpeedRot = m_sen.GetGyro();
			int nElapsedSR = GetElapsedTimeUSSince(&gyro);
			gettimeofday(&gyro, NULL);


			m_fSpeed.x += m_fAccel.x*nElapsedAX/1000.f;
			m_fSpeed.y += m_fAccel.y*nElapsedAY/1000.f;
			m_fSpeed.z += (m_fAccel.z-m_fGravityAccel)*nElapsedAZ/1000.f;

			m_fPos.x += m_fSpeed.x*nElapsedAX/1000.f;
			m_fPos.y += m_fSpeed.y*nElapsedAY/1000.f;
			m_fPos.z += m_fSpeed.z*nElapsedAZ/1000.f;
			m_fPosRot += m_fSpeedRot*nElapsedSR/1000.f;
		}

	}




private:
	Sensors m_sen;

	int GetElapsedTimeUSSince(struct timeval* begin)
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		now = (now - *begin);
		return now.tv_sec*1000000 + now.tv_usec;
	}

	//In millimeters or degrees
	Vector3D<double> m_fPos;
	double m_fPosRot;

	//In meter.sec-1 or degrees.sec-1
	Vector3D<double> m_fSpeed;
	double m_fSpeedRot;

	//In meter.sec-2
	Vector3D<double> m_fAccel;

	double m_fGravityAccel;




	int nSleepTime;



};

#endif // SENSORMANAGER_HPP_INCLUDED
