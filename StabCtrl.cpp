#include "StabCtrl.hpp"

#include <unistd.h>
#include "Device.hpp"
#include "ConfigFile.hpp"
#include "MotorHdl.hpp"
#include "SensorHdl.hpp"

StabCtrl::StabCtrl(ConfigFile* cfg)
{
	m_fGlobalMotorSpeed=0;
	m_fZRotCompensation=0;

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
    if(!GetIsThreadQuitting())
        Stop();

	struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 10000000;
	for( ; m_fGlobalMotorSpeed>0 ; m_fGlobalMotorSpeed-=10/(float)fMaxSec)
	{
		ThreadProcess();
		nanosleep(&Delay, NULL);
	}
	m_fGlobalMotorSpeed=0;
	Device::GetMotors()->Stop();
}

void StabCtrl::ChangeMotorSpeed(float fAdd)
{
    m_fGlobalMotorSpeed+=fAdd;
    if(m_fGlobalMotorSpeed>100)m_fGlobalMotorSpeed=100;
    else if(m_fGlobalMotorSpeed<0)m_fGlobalMotorSpeed=0;
}


void StabCtrl::ThreadProcess()
{
	#ifdef DEBUG
    std::cout<<"S";
    #endif

	//roll & pitch stabilization
	Vector3D<float> fAcc(Device::GetSensors()->GetAcceleration());
	float fRotSpeed = Device::GetSensors()->GetAngularSpeed();

	Vector3D<float> vInclinaison(fAcc);
	vInclinaison.Normalize();//Normalize now will prevent to normalize each time PlanGetZAt is called

	m_fZRotCompensation+=m_fRotSensibility*fRotSpeed; //@note may need some better calculus ;)

	float fSpeed[4] = {
						m_fGlobalMotorSpeed ,//+ m_fZRotCompensation/2.f - vInclinaison.PlanGetZAt(28.25, 28.25),
						m_fGlobalMotorSpeed ,//+ m_fZRotCompensation/2.f - vInclinaison.PlanGetZAt(28.25, -28.25),
						m_fGlobalMotorSpeed ,//+ m_fZRotCompensation/2.f - vInclinaison.PlanGetZAt(-28.25, -28.25),
						m_fGlobalMotorSpeed //+ m_fZRotCompensation/2.f - vInclinaison.PlanGetZAt(-28.25, 28.25)
					};

//    	std::cout<<"\e[31m#"<<1<<"="<<Device::GetMotors()->GetSpeed(1)<<"#\e[m";
//    	std::cout<<"\e[31m#"<<2<<"="<<Device::GetMotors()->GetSpeed(2)<<"#\e[m";
//    	std::cout<<"\e[31m#"<<3<<"="<<Device::GetMotors()->GetSpeed(3)<<"#\e[m";
//    	std::cout<<"\e[31m#"<<4<<"="<<Device::GetMotors()->GetSpeed(4)<<"#\e[m";

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
