#include "SensorHdl.hpp"

#include "ConfigFile.hpp"
#include "Sensor.hpp"
#include "timetools.h"
#include <unistd.h>


SensorHdl::SensorHdl(const ConfigFile* cfg) : LivingThread("SensorHandler"), m_fPos(0,0,0), m_fSpeed(0,0,0), m_fAccel(0,0,0)
{
    m_sen = new Sensor(cfg);
    ResetPosition();
    ResetSpeed();

    m_IntegSleepTime.tv_sec=0;
    m_IntegSleepTime.tv_nsec = cfg->GetValue<float>("SEN_IntegrationDelay")*1000000.0;
	if(m_IntegSleepTime.tv_nsec>=1000000000)
	{
		std::cerr<<"\e[31mError : Please set a SEN_IntegrationDelay value < 1000\e[m"<<std::endl;
		sleep(-1);
	}
}

void SensorHdl::ResetPosition()
{
    m_fPos.Set(0,0,0);
    m_fPosRot=0;
}

void SensorHdl::ResetSpeed()
{
    m_fSpeed.Set(0,0,0);
    m_fSpeedRot=0;

    m_vGravityAccel = Vector3D<float>(m_sen->GetAcceleroX(), m_sen->GetAcceleroY(), m_sen->GetAcceleroZ());
}

Vector3D<float> SensorHdl::GetPosition()
{
    return m_fPos;
}

float SensorHdl::GetAngularPosition()
{
    return m_fPosRot;
}

Vector3D<float> SensorHdl::GetSpeed()
{
    return m_fSpeed;
}

float SensorHdl::GetAngularSpeed()
{
    return m_fPosRot;
}

Vector3D<float> SensorHdl::GetAcceleration()
{
    return m_fAccel;
}

Vector3D<float> SensorHdl::GetGravityAcceleration()
{
    return m_vGravityAccel;
}




int SensorHdl::GetElapsedTimeUSSince(struct timeval* begin)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    now = (now - *begin);
    return now.tv_sec*1000000 + now.tv_usec;
}


void SensorHdl::OnThreadStart()
{
    gettimeofday(&m_dateAccelX, NULL);
    m_dateAccelY = m_dateAccelX; m_dateAccelZ = m_dateAccelX; m_dateGyro = m_dateAccelX;
}

void SensorHdl::ThreadProcess()
{
	#ifdef DEBUG
    std::cout<<"I";
    #endif

    //Get the values
    m_fAccel.x = m_sen->GetAcceleroX();
    int nElapsedAX = GetElapsedTimeUSSince(&m_dateAccelX);
    gettimeofday(&m_dateAccelX, NULL);

    m_fAccel.y = m_sen->GetAcceleroY();
    int nElapsedAY = GetElapsedTimeUSSince(&m_dateAccelY);
    gettimeofday(&m_dateAccelY, NULL);

    m_fAccel.z = m_sen->GetAcceleroZ();
    int nElapsedAZ = GetElapsedTimeUSSince(&m_dateAccelZ);
    gettimeofday(&m_dateAccelZ, NULL);

    m_fSpeedRot = m_sen->GetGyro();
    int nElapsedSR = GetElapsedTimeUSSince(&m_dateGyro);
    gettimeofday(&m_dateGyro, NULL);

	//Integrate the values
    m_fSpeed.x += (m_fAccel.x-m_vGravityAccel.x)*nElapsedAX/1000.f;
    m_fSpeed.y += (m_fAccel.y-m_vGravityAccel.y)*nElapsedAY/1000.f;
    m_fSpeed.z += (m_fAccel.z-m_vGravityAccel.z)*nElapsedAZ/1000.f;

    m_fPos.x += m_fSpeed.x*nElapsedAX/1000.f;
    m_fPos.y += m_fSpeed.y*nElapsedAY/1000.f;
    m_fPos.z += m_fSpeed.z*nElapsedAZ/1000.f;
    m_fPosRot += m_fSpeedRot*nElapsedSR/1000.f;

    nanosleep(&m_IntegSleepTime, NULL);
}
