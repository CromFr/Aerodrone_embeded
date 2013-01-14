#include "Motors.hpp"

#include <WiringPi.h>
#include "ConfigFile.hpp"

//==========================================================================
Motors::Motor::Motor(int nPin)
{
    m_nPin = nPin;
    pinMode(nPin, OUTPUT);
    digitalWrite(nPin, 0);

    m_fSpeed = 0;
}

void Motors::Motor::SetSpeed(float fSpeed100)
{
    m_fSpeed = fSpeed100;
}

float Motors::Motor::GetSpeed()
{
    return m_fSpeed;
}

int Motors::Motor::GetPin()
{
    return m_nPin;
}


//==========================================================================
Motors::Motors(const ConfigFile* cfg)
{
    for(int i=0 ; i<4 ; i++)
        m_mot[i] = new Motor(cfg->GetValue<int>("PIN_Motors", i));

    m_fMotorMinSpeed = cfg->GetValue<float>("MOT_MinSpeed")/100.f;

    m_fPWMFreq = cfg->GetValue<float>("MOT_PwmFreq");

    m_fProcessPrecision = cfg->GetValue<float>("MOT_PwmPrecision") * 100.0;

    m_bQuitThread = true;


}
Motors::~Motors()
{
    for(int i=0 ; i<4 ; i++)
        delete m_mot[i];
}



void Motors::PWMThread()
{
    int nDelayPwmUS = (1.f/m_fPWMFreq)*1000000;

    float fTmp;
    struct timeval begin, current;
    struct timespec sleepTime; sleepTime.tv_sec=0; sleepTime.tv_nsec=m_fProcessPrecision*1000.0;
    float fCalculSimplification = (100-m_fMotorMinSpeed)/10000;
    while(!m_bQuitThread)
    {
        //Init timer
        gettimeofday(&begin, NULL);

        //Init pins to LO
        for(short i=0 ; i<4 ; i++)
            digitalWrite(m_mot[i]->GetPin(), false);


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
                fDelayMotUS[i] = nDelayPwmUS*(1-fTmp*fCalculSimplification+0.35);
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
                    digitalWrite(m_mot[i]->GetPin(), true);
                    bMot[i] = false;
                }
            }
            nanosleep(&sleepTime, NULL);
        }while(nElapsedTimeUS <= nDelayPwmUS);
    }

    for(short i=0 ; i<4 ; i++)
    {
        digitalWrite(m_mot[i]->GetPin(), false);
    }

}


void Motors::StartThread()
{
    if(m_bQuitThread)
    {
        std::cout<<"-> Starting Motors Thread !"<<std::endl;
        m_bQuitThread = false;
        pthread_create(&m_thread, NULL , Motors::MotorsThreadWrapper, this);
    }
    else
    {
        std::cerr<<"Motors::StartThread() : A thread is already running"<<std::endl;
    }
}


void Motors::QuitThread()
{
    if(!m_bQuitThread)
    {
        m_bQuitThread = true;
        std::cout<<"/!\\ > Exiting Motors Thread !"<<std::endl;
    }
    else
    {
        std::cerr<<"Motors::StartThread() : There is no thread to quit / The thread is currently exiting"<<std::endl;
    }
}


void Motors::SetSpeed(int motID, float fPercent)
{
    m_mot[motID-1]->SetSpeed(fPercent);
}

float Motors::GetSpeed(int motID)
{
    return m_mot[motID-1]->GetSpeed();
}


void* Motors::MotorsThreadWrapper(void* obj)
{
    Motors* mot = reinterpret_cast<Motors*>(obj);
    mot->PWMThread();
    return 0;
}
