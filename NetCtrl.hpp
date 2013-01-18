#ifndef NETCTRL_HPP_INCLUDED
#define NETCTRL_HPP_INCLUDED

#include "LivingThread.hpp"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Device;
class ConfigFile;

/**
@brief [Controller] Process network data and controlls the device with
**/
class NetCtrl : public LivingThread
{
public:
    NetCtrl(ConfigFile* cfg);

private:
    void ProcessNetData(const char* data);

    void OnThreadStart();
	virtual void ThreadProcess();
    void OnThreadStop();


	int m_nSockPort;
	int m_sockServer, m_sockClient;
	struct sockaddr_in m_addrServer, m_addrClient;


};


#endif // NETCTRL_HPP_INCLUDED
