#include "StabCtrl.hpp"

#include "ConfigFile.hpp"
#include "Device.hpp"
#include "Motors.hpp"
#include "SensorManager.hpp"

StabCtrl::StabCtrl(ConfigFile* cfg)
{
	m_fGlobalMotorSpeed=0;
	m_fZRotCompensation=0;

	m_sleepTime.tv_sec = 0;
	m_sleepTime.tv_nsec = cfg->GetValue<float>("STA_SleepTime")*1000.0;
	m_fSensibility = cfg->GetValue<float>("STA_Sensibility");
	m_fRotSensibility = cfg->GetValue<float>("STA_RotSensibility");

}

void StabCtrl::LandRoutine(float fMaxSec)
{
	struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 10000000;
	for( ; m_fGlobalMotorSpeed>0 ; m_fGlobalMotorSpeed-=10/(float)fMaxSec)
	{
		ThreadProcess();
		nanosleep(&Delay, NULL);
	}
	m_fGlobalMotorSpeed=0;
	Device::GetMotors()->QuitThread();
	Stop();
}


void StabCtrl::ThreadProcess()
{
    std::cout<<"S";

	//roll & pitch stabilization
	Vector3D<double> fAcc(Device::GetSensors()->GetAcceleration());
	float fRotSpeed =Device::GetSensors()->GetAngularSpeed();

	Vector3D<double> vInclinaison(fAcc);
	vInclinaison.Normalize();

	m_fZRotCompensation+=m_fRotSensibility*fRotSpeed; //@note may need some better calculus ;)

	double fSpeed[4] = {
						m_fGlobalMotorSpeed + m_fZRotCompensation/2.f - vInclinaison.PlanGetZAt(28.25, 28.25),
						m_fGlobalMotorSpeed + m_fZRotCompensation/2.f - vInclinaison.PlanGetZAt(28.25, -28.25),
						m_fGlobalMotorSpeed + m_fZRotCompensation/2.f - vInclinaison.PlanGetZAt(-28.25, -28.25),
						m_fGlobalMotorSpeed + m_fZRotCompensation/2.f - vInclinaison.PlanGetZAt(-28.25, 28.25)
					};


	//Makes impossible to have speeds under 0 or over 100
	for(int i=0 ; i<4 ; i++)
	{
		if(fSpeed[i]>100.0)
		{
			float fDiff = fSpeed[i]-100.0;
			for(int j=0 ; j<4 ; j++)
				fSpeed[j] -= fDiff;
		}
		else
		{
			float fDiff = -fSpeed[i];
			for(int j=0 ; j<4 ; j++)
				fSpeed[j] += fDiff;
		}
	}

	//Set the speed
	for(int i=0 ; i<4 ; i++)
	{
		Device::GetMotors()->SetSpeed(i+1, fSpeed[i]);
	}

}

void StabCtrl::PostThreadProcess()
{
	nanosleep(&m_sleepTime, NULL);
}
