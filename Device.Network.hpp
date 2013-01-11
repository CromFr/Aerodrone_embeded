
#define NET_INFOREQUEST 1
#define NET_INFODATA 2
#define NET_COMMAND_MOVE 2


public:
	void StartNetwork()
	{
		if(m_bQuitNetworkThread)
		{
			m_bQuitNetworkThread=false;

			//Init server socket
            m_sockServer = socket(AF_INET, SOCK_STREAM, 0);
            if(m_sockServer < 0)
            {
                std::cerr<<__FILE__<<" @ "<<__LINE__<<" : Error while opening server socket"<<std::endl;
                return;
            }

            //Init server address
            bzero((char *) &m_addrServer, sizeof(m_addrServer));
            m_addrServer.sin_family = AF_INET;
            m_addrServer.sin_addr.s_addr = INADDR_ANY;
            m_addrServer.sin_port = htons(m_nSockPort);

            if(bind(m_sockServer, (struct sockaddr *) &m_addrServer, sizeof(m_addrServer)) < 0)
            {
                std::cerr<<__FILE__<<" @ "<<__LINE__<<" : Error while binding server socket"<<std::endl;
                return;
            }

            listen(m_sockServer,5);


			pthread_create(&m_threadNetwork, NULL , Device::NetworkThreadWrapper, this);
		}
		else
		{
			std::cerr<<__FILE__<<" @ "<<__LINE__<<" : The thread is already running"<<std::endl;
		}
	}
private:
	static void* NetworkThreadWrapper(void* obj)
	{
		Device* dev = reinterpret_cast<Device*>(obj);
		dev->NetworkThread();
		return 0;
	}


	void NetworkThread()
	{
		while(!m_bQuitNetworkThread)
		{
			ProcessNetwork();
		}
	}

	void ProcessNetwork()
	{
        std::clog<<"Waiting for client connection"<<std::endl;
        struct sockaddr_in m_addrClient;
        socklen_t addrlenClient = sizeof(m_addrClient);
        int m_sockClient = accept(m_sockServer, (struct sockaddr *) &m_addrClient, &addrlenClient);
        if (m_sockClient < 0)
            std::cerr<<__FILE__<<" @ "<<__LINE__<<" : Error while accepting client connexion"<<std::endl;

		short addrA = m_addrClient.sin_addr.s_addr/16777216;// /2^24
		short addrB = (m_addrClient.sin_addr.s_addr-addrA*16777216)/65536;// /2^16
		short addrC = (m_addrClient.sin_addr.s_addr-addrA*16777216-addrB*65536)/256;// /2^8
		short addrD = m_addrClient.sin_addr.s_addr-addrA*16777216-addrB*65536-addrC*256;
        std::clog<<"Connection from client: "<<addrA<<"."<<addrB<<"."<<addrC<<"."<<addrD<<std::endl;

		while(!m_bQuitNetworkThread && m_sockClient>0)
		{
			int nPacketSize;

			int n = read(m_sockClient,(char*)(&nPacketSize),2);
            if (n < 0)
                std::cerr<<"Error while reading client socket n"<<std::endl;

            char cData[nPacketSize];
			int m = read(m_sockClient,(char*)(&cData),nPacketSize);
            if (m < 0)
                std::cerr<<"Error while reading client socket m"<<std::endl;

            ProcessNetData(cData);
		}

        close(m_sockClient);

	}

	void ProcessNetData(const char* data)
	{
        std::istringstream sData(data);

        short nAction;
        sData.read((char*)(&nAction), 2);

        if(nAction == NET_INFOREQUEST)
        {
            std::ostringstream sToSend;

            //Reserve space for packet size
            sToSend.write(NULL, 2);

            nAction = NET_INFODATA;
            sToSend.write((char*)(&nAction), 1);

            //TODO Check if the written value is exact and not trimmed
            int nValue = m_mot->GetSpeed(1)*655.36;
            sToSend.write((char*)(&nValue), 2);
            nValue = m_mot->GetSpeed(2)*655.36;
            sToSend.write((char*)(&nValue), 2);
            nValue = m_mot->GetSpeed(3)*655.36;
            sToSend.write((char*)(&nValue), 2);
            nValue = m_mot->GetSpeed(4)*655.36;
            sToSend.write((char*)(&nValue), 2);

            nValue = m_sen->GetAcceleration().x*2184.53;
            sToSend.write((char*)(&nValue), 2);
            nValue = m_sen->GetAcceleration().y*2184.53;
            sToSend.write((char*)(&nValue), 2);
            nValue = m_sen->GetAcceleration().z*2184.53;
            sToSend.write((char*)(&nValue), 2);
            nValue = m_sen->GetAngularSpeed()*2184.53;
            sToSend.write((char*)(&nValue), 2);//TODO Change the * value

            //Write packet size
            sToSend.seekp(std::ios_base::beg);
            nValue = sToSend.str().length()-2;
            sToSend.write((char*)(&nValue), 2);

            write(m_sockClient,sToSend.str().c_str(),sToSend.str().length());
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

	int m_nSockPort;
	int m_sockServer, m_sockClient;
	struct sockaddr_in m_addrServer, m_addrClient;


	bool m_bQuitNetworkThread;
	pthread_t m_threadNetwork;


