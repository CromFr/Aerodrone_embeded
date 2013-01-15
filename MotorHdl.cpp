#include "MotorHdl.hpp"

#include "Motor.hpp"
#include "ConfigFile.hpp"


//==========================================================================
MotorHdl::MotorHdl(const ConfigFile* cfg)
{
    for(int i=0 ; i<4 ; i++)
        m_mot[i] = new Motor(cfg->GetValue<int>("PIN_Motors", i));


    m_fMotorMinSpeed = cfg->GetValue<float>("MOT_MinSpeed")/100.f;
    m_fPWMFreq = cfg->GetValue<float>("MOT_PwmFreq");

    m_PWMCheckSleep.tv_sec=0; m_PWMCheckSleep.tv_nsec=cfg->GetValue<float>("MOT_PwmPrecision") * 100000;
}
MotorHdl::~MotorHdl()
{
    for(int i=0 ; i<4 ; i++)
        delete m_mot[i];
}



void MotorHdl::ThreadProcess()
{
    int nDelayPwmUS = (1.f/m_fPWMFreq)*1000000;

    float fTmp;
    struct timeval begin, current;

    //Init pins to LO
    for(short i=0 ; i<4 ; i++)
        m_mot[i]->SetPin(false);

    //Start timer
    gettimeofday(&begin, NULL);


    //Calculating delays for the motors
    int fDelayMotUS[4];
    for(short i=0 ; i<4 ; i++)
    {
        fTmp = m_mot[i]->GetSpeed();
        if(fTmp == 0)
            fDelayMotUS[i] = 0;
        else
        {
            //fDelayMotUS[i] = nDelayPwmUS*(1-((fTmp/100.f)*(100.f-m_fMotorMinSpeed)+35.f)/100.f);
            fDelayMotUS[i] = nDelayPwmUS*(1-fTmp*(100-m_fMotorMinSpeed)/10000+0.35);
        }
    }


    //Processing pwm
    bool bMot[4] = {true};
    int nElapsedTimeUS;
    do
    {
        gettimeofday(&current, NULL);

        if(current.tv_usec < begin.tv_usec)current.tv_usec+=1000000;
        nElapsedTimeUS = current.tv_usec - begin.tv_usec;

        for(short i=0 ; i<4 ; i++)
        {
            if(bMot[i] && nElapsedTimeUS >= fDelayMotUS[i])
            {
                m_mot[i]->SetPin(true);
                bMot[i] = false;
            }
        }
        nanosleep(&m_PWMCheckSleep, NULL);
    }while(nElapsedTimeUS <= nDelayPwmUS);



}


void MotorHdl::OnThreadEnd()
{
    for(short i=0 ; i<4 ; i++)
        m_mot[i]->SetPin(false);
}


void MotorHdl::SetSpeed(int motID, float fPercent)
{
    m_mot[motID-1]->SetSpeed(fPercent);
}

float MotorHdl::GetSpeed(int motID)
{
    return m_mot[motID-1]->GetSpeed();
}
