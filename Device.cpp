#include "Device.hpp"

#include <WiringPi.h>
#include "StabCtrl.hpp"
#include "NetCtrl.hpp"
#include "Motors.hpp"
#include "SensorManager.hpp"
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

    //===> No errors during loading
    m_instance = this;


    //Hardware
    m_mot = new Motors(cfg);
    m_sen = new SensorManager(cfg);

    m_mot->StartThread();
    m_sen->StartThread();

    //Stabilisation
    m_stabctrl = new StabCtrl(cfg);
    //m_stabctrl->Start();

    m_netctrl = new NetCtrl(cfg);
    //m_netctrl->Start();



}
Device::~Device()
{
    delete m_mot;
    delete m_sen;

    delete m_stabctrl;
    delete m_netctrl;
}



void Device::StartupRoutine()
{
    struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 100000000;//every 0.1sec
    for(int nMotorSpeed=0 ; nMotorSpeed<93 ; nMotorSpeed++)
    {
        m_mot->SetSpeed(1, nMotorSpeed);
        m_mot->SetSpeed(2, nMotorSpeed);
        m_mot->SetSpeed(3, nMotorSpeed);
        m_mot->SetSpeed(4, nMotorSpeed);
        nanosleep(&Delay, NULL);
    }
    std::cout<<"Motors at 100% !"<<std::endl;
}



void Device::OnErrorLand()
{
    m_stabctrl->LandRoutine(4);
}


void Device::OnCriticalErrorLand()
{
    m_stabctrl->LandRoutine(1);
}
