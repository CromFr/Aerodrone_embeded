#include "NetCtrl.hpp"

#include <cstdlib>
#include <cstdint>

#include "Device.hpp"
#include "Motors.hpp"
#include "SensorManager.hpp"
#include "ConfigFile.hpp"



#define NET_INFOREQUEST (uint8_t)1
#define NET_INFODATA (uint8_t)2
#define NET_COMMAND_MOVE (uint8_t)3

NetCtrl::NetCtrl(ConfigFile* cfg)
{
	m_nSockPort = cfg->GetValue<int>("NET_Port");
}


void NetCtrl::OnThreadStart()
{
    //Init server socket
    m_sockServer = socket(AF_INET, SOCK_STREAM, 0);
    if(m_sockServer < 0)
    {
        std::cerr<<__FILE__<<" @ "<<__LINE__<<" : Error while opening server socket ("/*<<strerror(errno)*/<<")"<<std::endl;
        sleep(5);
    }

    //Init server address
    bzero((char *) &m_addrServer, sizeof(m_addrServer));
    m_addrServer.sin_family = AF_INET;
    m_addrServer.sin_addr.s_addr = INADDR_ANY;
    m_addrServer.sin_port = htons(m_nSockPort);

    if(bind(m_sockServer, (struct sockaddr *) &m_addrServer, sizeof(m_addrServer)) < 0)
    {
        std::cerr<<__FILE__<<" @ "<<__LINE__<<" : Error while binding server socket ("/*<<strerror(errno)*/<<")"<<std::endl;
        sleep(5);
    }
    listen(m_sockServer,5);

}



void NetCtrl::ThreadProcess()
{
    std::clog<<"Waiting for client connection..."<<std::endl;
    struct sockaddr_in m_addrClient;
    socklen_t addrlenClient = sizeof(m_addrClient);
    m_sockClient = accept(m_sockServer, (struct sockaddr *) &m_addrClient, &addrlenClient);
    if (m_sockClient < 0)
    {
        std::cerr<<__FILE__<<" @ "<<__LINE__<<" : Error while accepting client connexion ("/*<<strerror(errno)*/<<")"<<std::endl;
        sleep(5);
    }

    short addrA = m_addrClient.sin_addr.s_addr/16777216;// /2^24
    short addrB = (m_addrClient.sin_addr.s_addr-addrA*16777216)/65536;// /2^16
    short addrC = (m_addrClient.sin_addr.s_addr-addrA*16777216-addrB*65536)/256;// /2^8
    short addrD = m_addrClient.sin_addr.s_addr-addrA*16777216-addrB*65536-addrC*256;
    std::clog<<"Connection from client: "<<addrD<<"."<<addrC<<"."<<addrB<<"."<<addrA<<std::endl;

    while(!GetIsThreadQuitting() && m_sockClient>=0)
    {
        uint16_t nPacketSize;

        int n = read(m_sockClient,(char*)(&nPacketSize),2);
        if (n <= 0)break;

        char cData[nPacketSize];

        int m = read(m_sockClient,(char*)(&cData),nPacketSize);
        if (m <= 0)break;

        ProcessNetData(cData);
    }

    close(m_sockClient);
    std::clog<<"Client connection closed"<<std::endl;
}

void WriteSSTream16(std::stringstream& ss, uint16_t i)
{
    uint16_t nTmp;
    if(i==0)
    {
        ss.write((char*)(&(nTmp=0)), 2);
    }
    else if(i<256)
    {
        ss.write((char*)(&(nTmp=0)), 1);
        ss.write((char*)(&(nTmp=i)), 1);
    }
    else
    {
        ss.write((char*)(&(nTmp=i/256)), 1);
        ss.write((char*)(&(nTmp=i-i/256)), 1);
    }
}
void WriteSSTream8(std::stringstream& ss, uint8_t i)
{
    ss.write((char*)(&(i)), 1);
}

void NetCtrl::ProcessNetData(const char* data)
{
    std::istringstream sData(data);

    uint8_t nAction;
    sData.read((char*)(&nAction), 1);

    if(nAction == NET_INFOREQUEST)
    {
        std::cout<<"#IR#";

        std::stringstream sToSend;

        //Reserve space for packet size
        WriteSSTream16(sToSend, 0);

        WriteSSTream8(sToSend, NET_INFODATA);

        //Motors
        // The sensors values are in [0, +100] G
        WriteSSTream16(sToSend, Device::GetMotors()->GetSpeed(1)*655.36);
        WriteSSTream16(sToSend, Device::GetMotors()->GetSpeed(2)*655.36);
        WriteSSTream16(sToSend, Device::GetMotors()->GetSpeed(3)*655.36);
        WriteSSTream16(sToSend, Device::GetMotors()->GetSpeed(4)*655.36);

        //Sensors
        // The sensors values are in [-3, +3] G
        //TODO Optimize the values for moving in m.s-2
        WriteSSTream16(sToSend, Device::GetSensors()->GetAcceleration().x);
        WriteSSTream16(sToSend, Device::GetSensors()->GetAcceleration().y);
        WriteSSTream16(sToSend, Device::GetSensors()->GetAcceleration().z);
        WriteSSTream16(sToSend, Device::GetSensors()->GetAngularSpeed());

        //Write packet size
        uint16_t nPacketSize = sToSend.tellp();
        sToSend.seekp(std::ios_base::beg);
        WriteSSTream16(sToSend, nPacketSize-2);

        //Sending packet
        int m = write(m_sockClient, sToSend.str().c_str(), sToSend.str().length());
        if (m <= 0)std::cerr<<__FILE__<<" @ "<<__LINE__<<" : Error while sending InfoData ("/*<<strerror(errno)*/<<")"<<std::endl;
    }
    else if(nAction == NET_INFODATA)
    {
        //Should not happen
        std::cerr<<__FILE__<<" @ "<<__LINE__<<" : Received NET_INFODATA, and it should not happen"<<std::endl;
    }
    else if(nAction == NET_COMMAND_MOVE)
    {
        //TODO Complete this
    }


}
