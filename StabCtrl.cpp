#include "StabCtrl.hpp"

#include <unistd.h>
#include "Device.hpp"
#include "ConfigFile.hpp"
#include "MotorHdl.hpp"
#include "SensorHdl.hpp"

StabCtrl::StabCtrl(ConfigFile* cfg) :  LivingThread("StabController")
{
	m_sleepTime.tv_sec = 0;
	m_sleepTime.tv_nsec = cfg->GetValue<float>("STA_SleepTime")*1000.0;
	if(m_sleepTime.tv_nsec>=1000000000)
	{
		std::cerr<<"\e[31mError : Please set a STA_SleepTime value < 1 000 000 000\e[m"<<std::endl;
		sleep(-1);
	}
	else if(m_sleepTime.tv_nsec<0)
	{
		std::cerr<<"\e[31mError : Please set a STA_SleepTime value >= 0\e[m"<<std::endl;
		sleep(-1);
	}

	m_fSensibility = cfg->GetValue<float>("STA_Sensibility");
	if(m_fSensibility<=0)
	{
		std::cerr<<"\e[31mError : Please set a STA_Sensibility value > 0\e[m"<<std::endl;
		sleep(-1);
	}
	m_fRotSensibility = cfg->GetValue<float>("STA_RotSensibility");
	if(m_fRotSensibility<=0)
	{
		std::cerr<<"\e[31mError : Please set a STA_RotSensibility value > 0\e[m"<<std::endl;
		sleep(-1);
	}
}

void StabCtrl::LandRoutine(float fMaxSec)
{

	std::cerr<<"\e[31m/!\\ ==> The device is landing\e[m"<<std::endl;
    if(!GetIsThreadQuitting())
        Stop();
	std::cerr<<"-> StabCrtl \e[31m[Stopped]\e[m (now controlled manually)"<<std::endl;

	struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 10000000;
	for( ; m_fGlobalMotorSpeed>0 ; m_fGlobalMotorSpeed-=10/(float)fMaxSec)
	{
		ThreadProcess();
		nanosleep(&Delay, NULL);
	}
	m_fGlobalMotorSpeed=0;
	std::cerr<<"-> MotorHandler \e[31m[Stopped]\e[m"<<std::endl;
	Device::GetMotors()->Stop();
	std::cerr<<"\e[32m--> Landing successful\e[m"<<std::endl;
}

void StabCtrl::ChangeMotorSpeed(float fAdd)
{
    m_fGlobalMotorSpeed+=fAdd;
    if(m_fGlobalMotorSpeed>100)m_fGlobalMotorSpeed=100;
    else if(m_fGlobalMotorSpeed<0)m_fGlobalMotorSpeed=0;
}
void StabCtrl::SetGlobalMotorSpeed(float fValue)
{
    m_fGlobalMotorSpeed=fValue;
}

void StabCtrl::SetAnglularCompensation(float* fRotXDeg, float* fRotYDeg)
{
	if(fRotXDeg!=0)m_fXAngleCompensation = *fRotXDeg;
	if(fRotYDeg!=0)m_fYAngleCompensation = *fRotYDeg;
}

void StabCtrl::OnThreadStart()
{
	m_fXAngleCompensation=0;
	m_fYAngleCompensation=0;
	m_fGlobalMotorSpeed=0;
	m_fZRotCompensation=0;
}

void StabCtrl::ThreadProcess()
{
	#ifdef DEBUG
    std::cout<<"S";
    #endif

	//----------------------------------------------------------------------------------
	Vector3D<float> vSpeed(Device::GetSensors()->GetSpeed());

	m_fGlobalMotorSpeed += (m_vTargetSpeed.z - vSpeed.z)*m_fSensibility;
	float fDiffY = (m_vTargetSpeed.y - vSpeed.y)*m_fSensibility/2.f;
	float fDiffX = (m_vTargetSpeed.x - vSpeed.x)*m_fSensibility/2.f;

	m_fZRotCompensation += m_fRotSensibility*Device::GetSensors()->GetAngularSpeed()/2.f; //@note may need some better calculus ;)

	/** Drone layout
	4     1
	 \   /	 y
	  \ /	 |
	  [ ]	 o--x
	  / \	z
	 /   \
	3     2
	**/

	float fSpeed[4] = {
						m_fGlobalMotorSpeed + m_fZRotCompensation - fDiffY - fDiffX,
						m_fGlobalMotorSpeed - m_fZRotCompensation + fDiffY - fDiffX,
						m_fGlobalMotorSpeed + m_fZRotCompensation + fDiffY + fDiffX,
						m_fGlobalMotorSpeed - m_fZRotCompensation - fDiffY + fDiffX
					};


	//----------------------------------------------------------------------------------
	//Make impossible to have speeds under 0% or over 100%
	float fMaxValue = 0;
	for(int i=0 ; i<4 ; i++)if(fSpeed[i]>fMaxValue)fMaxValue = fSpeed[i];
	float fMinValue = 100;
	for(int i=0 ; i<4 ; i++)if(fSpeed[i]<fMinValue)fMinValue = fSpeed[i];

    if(fMaxValue>100)
    {
        float fDiff = fMaxValue-100;
        for(int i=0 ; i<4 ; i++)
        {
            fSpeed[i]-= fDiff;
        }
    }
    else if(fMinValue<0)
    {
        float fDiff = -fMinValue;
        for(int i=0 ; i<4 ; i++)
        {
            fSpeed[i]+= fDiff;
        }
    }
    //

	//Set the speed
	for(int i=0 ; i<4 ; i++)
	{
		Device::GetMotors()->SetSpeed(i+1, fSpeed[i]);
	}




	nanosleep(&m_sleepTime, NULL);

}
