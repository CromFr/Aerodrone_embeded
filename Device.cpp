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
    std::clog<<"\e[33mInitializing Device...\e[m"<<std::endl;

	//Load configuration file (config.cfg)
    ConfigFile* cfg = new ConfigFile;
    ConfigFile::Error nError = cfg->Load("./config.cfg", false);
    if(nError != ConfigFile::no_error)
    {
        std::cerr<<"-> \e[31mConfigFile loading error (./config.cfg) : "<<ConfigFile::GetErrorString(nError)<<"\e[m"<<std::endl;
        sleep(-1);
    }
    else
        std::clog<<"-> ConfigFile \e[32m[ok]\e[m"<<std::endl;

	//Init WiringPi
    if (wiringPiSetup() == -1)
    {
        std::cerr<<"-> \e[31mWiringPi setup failed ! ;(\e[m"<<std::endl;
        sleep(-1);
    }
    else
        std::clog<<"-> WiringPi \e[32m[ok]\e[m"<<std::endl;


	//Init buzzer
    m_nBuzzerPin = cfg->GetValue<int>("PIN_Buzzer");
    softToneCreate(m_nBuzzerPin);


    //===> No errors during loading

    //Register device instance
    m_instance = this;

    //Start Hardware handlers
    m_mot = new MotorHdl(cfg);
    m_mot->Start();
    std::clog<<"-> MotorHdl \e[32m[Started]\e[m"<<std::endl;

    m_sen = new SensorHdl(cfg);
    m_sen->Start();
    std::clog<<"-> SensorHdl \e[32m[Started]\e[m"<<std::endl;


    //Start Stabilisation
    m_stabctrl = new StabCtrl(cfg);
    m_stabctrl->Start();
    std::clog<<"-> StabCtrl \e[32m[Started]\e[m"<<std::endl;

    //Start Network
    m_netctrl = new NetCtrl(cfg);
    m_netctrl->Start();
    std::clog<<"-> NetCtrl \e[32m[Started]\e[m"<<std::endl;

	//Bip the "Begin" tune
    int nSeq[] = {200,400,400};
    BipRoutine(nSeq, 3);

	//Drop the config file
    delete cfg;

    std::clog<<"-> \e[32mDevice Ready!\e[m"<<std::endl;
}

Device::~Device()
{

	//Bip the "End" tune
    int nSeq[] = {400,200,200};
    BipRoutine(nSeq, 3);

	//Delete controllers
    std::clog<<"-> Deleting Stabilization Controller"<<std::endl;
    delete m_stabctrl;
    std::clog<<"-> Deleting Network Controller"<<std::endl;
    delete m_netctrl;

	//Delete handlers
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
    struct timespec Delay; Delay.tv_sec=0; Delay.tv_nsec = 200000000;//every 0.2sec

    for(int i=0 ; i<nSize ; i++)
    {
        softToneWrite(m_nBuzzerPin, nSequence[i]);
        nanosleep(&Delay, NULL);
    }
    softToneWrite(m_nBuzzerPin, 0);
}
