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

		m_bQuitThread=true;

		m_SleepTime.tv_sec=0;
		m_SleepTime.tv_nsec = cfg.GetValue<float>("SEN_IntegrationDelay")*1000000.0;
	}

	/**
	@brief The current position will be the origin
	**/
	void ResetPosition()
	{
		m_fPos.Set(0,0,0);
		m_fPosRot=0;
	}

	/**
	@brief The sensor will belive the aerodrone is immobile
	@warning Is the device is moving, this will cause the device to keep moving this way
	**/
	void ResetSpeed()
	{
		m_fSpeed.Set(0,0,0);
		m_fSpeedRot=0;

		Vector3D<double> accel(m_sen.GetAcceleroX(), m_sen.GetAcceleroY(), m_sen.GetAcceleroZ());
		m_fGravityAccel = accel.GetLength();
	}

	/**
	@brief get the current position of the device from the origin, using the integrated speed
	@return The position in meters
	**/
	Vector3D<double> GetPosition()
	{
		return m_fPos;
	}

	/**
	@brief get the current angular position of the device from the origin, using the integrated angular speed
	@note Y axis is 0 and X is -PI/2
	@return The position in radians
	**/
	double GetAngularPosition()
	{
		return m_fPosRot;
	}

	/**
	@brief Get the current device speed, from the integrated acceleration
	@return The speed in meter/second
	**/
	Vector3D<double> GetSpeed()
	{
		return m_fSpeed;
	}


	/**
	@brief Get the current device angular speed, from the integrated angular acceleration
	@note Y axis is 0 and X is -PI/2
	@return The speed in radians/sec
	**/
	double GetAngularSpeed()
	{
		return m_fPosRot;
	}


	/**
	@brief Get the current device speed, from the integrated acceleration
	@return The acceleration in meter/second
	@note The gravity acceleration can be retrieved with GetGravityAcceleration()
	**/
	Vector3D<double> GetAcceleration()
	{
		return m_fAccel;
	}

	double GetGravityAcceleration()
	{
		return m_fGravityAccel;
	}

	void StartThread()
	{
		if(m_bQuitThread == false)
			std::cerr<<"SensorManager::StartThread() : A thread is already running"<<std::endl;
		else
		{
			std::cout<<"-> Starting SensorManager Thread !"<<std::endl;
			m_bQuitThread = false;
			pthread_create(&m_thread, NULL , SensorManager::ThreadWrapper, this);
		}
	}

	/**
	@brief Quit the thread !
	**/
	void QuitThread()
	{
		if(m_bQuitThread == true)
			std::cerr<<"SensorManager::QuitThread() : There is no thread to quit / The thread is currently exiting"<<std::endl;
		else
		{
			std::cout<<"/!\\ > Exiting SensorManager Thread !"<<std::endl;
			m_bQuitThread = true;
		}
	}




private:
	Sensors m_sen;

	struct timespec m_SleepTime;
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


	//============> Threading
	void Thread()
	{
		struct timeval accelX, accelY, accelZ, gyro;
		gettimeofday(&accelX, NULL);
		accelY = accelX; accelZ = accelX; gyro = accelX;
		while(!m_bQuitThread)
		{
			std::cout<<"Integ";
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

			nanosleep(&m_SleepTime, NULL);
		}
	}
	static void* ThreadWrapper(void* obj)
	{
		SensorManager* smgr = reinterpret_cast<SensorManager*>(obj);
		smgr->Thread();
		return 0;
	}
	pthread_t m_thread;
	bool m_bQuitThread;







	int nSleepTime;



};

#endif // SENSORMANAGER_HPP_INCLUDED
