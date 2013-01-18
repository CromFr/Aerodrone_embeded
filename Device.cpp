#include "Device.hpp"

#include "WiringPiWrap.h"
#include <softTone.h>
#include "StabCtrl.hpp"
#include "NetCtrl.hpp"
#include "MotorHdl.hpp"
#include "SensorHdl.hpp"
#include "ConfigFile.hpp"

Device* Device::m_instance;

Device::Device()
{
    std::cout<<"Initializing Device..."<<std::endl;

    ConfigFile* cfg = new ConfigFile;
    ConfigFile::Error nError = cfg->Load("./config.cfg", false);
    if(nError != ConfigFile::no_error)
    {
        std::cerr<<"-> ConfigFile loading error (./config.cfg) : "<<ConfigFile::GetErrorString(nError)<<std::endl;
        return;
    }
    else
        std::cout<<"-> ConfigFile ok"<<std::endl;

    if (wiringPiSetup() == -1)
    {
        std::cerr<<"-> WiringPi setup failed ! ;("<<std::endl;
        return;
    }
    else
        std::cout<<"-> WiringPi ok"<<std::endl;

    m_nBuzzerPin = cfg->GetValue<int>("PIN_Buzzer");
    softToneCreate(m_nBuzzerPin);


    //===> No errors during loading
    m_instance = this;


    //Hardware
    std::clog<<"-> MotorHdl"<<std::endl;
    m_mot = new MotorHdl(cfg);
    m_mot->Start();

    std::clog<<"-> SensorHdl"<<std::endl;
    m_sen = new SensorHdl(cfg);
    m_sen->Start();


    //Stabilisation
    std::clog<<"-> StabCtrl"<<std::endl;
    m_stabctrl = new StabCtrl(cfg);
    m_stabctrl->Start();

    //Network
    std::clog<<"-> NetCtrl"<<std::endl;
    m_netctrl = new NetCtrl(cfg);
    m_netctrl->Start();

    int nSeq[] = {200,400,400};
    BipRoutine(nSeq, 3);


    delete cfg;
}

Device::~Device()
{
    int nSeq[] = {400,200,200};
    BipRoutine(nSeq, 3);

    std::clog<<"-> Deleting Stabilization Controller"<<std::endl;
    delete m_stabctrl;
    std::clog<<"-> Deleting Network Controller"<<std::endl;
    delete m_netctrl;

    std::clog<<"-> Deleting Motor Handler"<<std::endl;
    delete m_mot;
    std::clog<<"-> Deleting Sensor Handler"<<std::endl;
    delete m_sen;
}



void Device::StartupRoutine()
{
    struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 100000000;//every 0.1sec
    for(int nMotorSpeed=0 ; nMotorSpeed<93 ; nMotorSpeed++)
    {
        m_stabctrl->ChangeMotorSpeed(1);
        nanosleep(&Delay, NULL);
    }
    std::cout<<"Motors at 100% !"<<std::endl;
}



void Device::OnErrorRoutine()
{
    m_stabctrl->LandRoutine(4);
}


void Device::OnCriticalErrorRoutine()
{
    m_stabctrl->LandRoutine(1);
}


void Device::BipRoutine(int* nSequence, int nSize)
{
    struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 300000000;//every 0.3sec

    for(int i=0 ; i<nSize ; i++)
    {
        softToneWrite(m_nBuzzerPin, nSequence[i]);
        nanosleep(&Delay, NULL);
    }
    softToneWrite(m_nBuzzerPin, 0);
}
