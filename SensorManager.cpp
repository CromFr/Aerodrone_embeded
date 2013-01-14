#include "SensorManager.hpp"

#include "ConfigFile.hpp"
#include "Sensors.hpp"
#include "timetools.h"


SensorManager::SensorManager(const ConfigFile* cfg) : m_fPos(0,0,0), m_fSpeed(0,0,0), m_fAccel(0,0,0)
{
    m_sen = new Sensors(cfg);
    ResetPosition();
    ResetSpeed();

    m_bQuitThread=true;

    m_SleepTime.tv_sec=0;
    m_SleepTime.tv_nsec = cfg->GetValue<float>("SEN_IntegrationDelay")*1000000.0;
}

void SensorManager::ResetPosition()
{
    m_fPos.Set(0,0,0);
    m_fPosRot=0;
}

void SensorManager::ResetSpeed()
{
    m_fSpeed.Set(0,0,0);
    m_fSpeedRot=0;

    m_vGravityAccel = Vector3D<double>(m_sen->GetAcceleroX(), m_sen->GetAcceleroY(), m_sen->GetAcceleroZ());
}

Vector3D<double> SensorManager::GetPosition()
{
    return m_fPos;
}

double SensorManager::GetAngularPosition()
{
    return m_fPosRot;
}

Vector3D<double> SensorManager::GetSpeed()
{
    return m_fSpeed;
}

double SensorManager::GetAngularSpeed()
{
    return m_fPosRot;
}

Vector3D<double> SensorManager::GetAcceleration()
{
    return m_fAccel;
}

Vector3D<double> SensorManager::GetGravityAcceleration()
{
    return m_vGravityAccel;
}

void SensorManager::StartThread()
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

void SensorManager::QuitThread()
{
    if(m_bQuitThread == true)
        std::cerr<<"SensorManager::QuitThread() : There is no thread to quit / The thread is currently exiting"<<std::endl;
    else
    {
        std::cout<<"/!\\ > Exiting SensorManager Thread !"<<std::endl;
        m_bQuitThread = true;
    }
}


int SensorManager::GetElapsedTimeUSSince(struct timeval* begin)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    now = (now - *begin);
    return now.tv_sec*1000000 + now.tv_usec;
}


void SensorManager::Thread()
{
    struct timeval accelX, accelY, accelZ, gyro;
    gettimeofday(&accelX, NULL);
    accelY = accelX; accelZ = accelX; gyro = accelX;
    while(!m_bQuitThread)
    {
        std::cout<<"I";
        m_fAccel.x = m_sen->GetAcceleroX();
        int nElapsedAX = GetElapsedTimeUSSince(&accelX);
        gettimeofday(&accelX, NULL);

        m_fAccel.y = m_sen->GetAcceleroY();
        int nElapsedAY = GetElapsedTimeUSSince(&accelY);
        gettimeofday(&accelY, NULL);

        m_fAccel.z = m_sen->GetAcceleroZ();
        int nElapsedAZ = GetElapsedTimeUSSince(&accelZ);
        gettimeofday(&accelZ, NULL);

        m_fSpeedRot = m_sen->GetGyro();
        int nElapsedSR = GetElapsedTimeUSSince(&gyro);
        gettimeofday(&gyro, NULL);


        m_fSpeed.x += (m_fAccel.x-m_vGravityAccel.x)*nElapsedAX/1000.f;
        m_fSpeed.y += (m_fAccel.y-m_vGravityAccel.y)*nElapsedAY/1000.f;
        m_fSpeed.z += (m_fAccel.z-m_vGravityAccel.z)*nElapsedAZ/1000.f;

        m_fPos.x += m_fSpeed.x*nElapsedAX/1000.f;
        m_fPos.y += m_fSpeed.y*nElapsedAY/1000.f;
        m_fPos.z += m_fSpeed.z*nElapsedAZ/1000.f;
        m_fPosRot += m_fSpeedRot*nElapsedSR/1000.f;

        nanosleep(&m_SleepTime, NULL);
    }
}


void* SensorManager::ThreadWrapper(void* obj)
{
    SensorManager* smgr = reinterpret_cast<SensorManager*>(obj);
    smgr->Thread();
    return 0;
}
