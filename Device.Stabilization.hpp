#ifndef DEVICE_STABILIZATION_HPP_INCLUDED
#define DEVICE_STABILIZATION_HPP_INCLUDED

public:
	void StartStabilization()
	{
		if(m_bQuitThread)
		{
			m_bQuitThread=false;
			pthread_create(&m_thread, NULL , Device::StabilizerThreadWrapper, this);

			m_mot->StartThread();
		}
		else
		{
			std::cerr<<__FILE__<<" @ "<<__LINE__<<" : The thread is already running"<<std::endl;
		}
	}
private:
	static void* StabilizerThreadWrapper(void* obj)
	{
		Device* dev = reinterpret_cast<Device*>(obj);
		dev->StabThread();
		return 0;
	}


	void StabThread()
	{
		while(!m_bQuitThread)
		{
			std::cout<<"Stab";
			ProcessStabilization();
			nanosleep(&sleepTime, NULL);
		}
	}

	void ProcessStabilization()
	{
		//roll & pitch stabilization
		Vector3D<double> fAcc(m_sen->GetAcceleration());
		float fRotSpeed = m_sen->GetAngularSpeed();

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
			m_mot->SetSpeed(i+1, fSpeed[i]);
		}
	}





	float m_fGlobalMotorSpeed;
	float m_fZRotCompensation;

	float m_fSensibility;
	float m_fRotSensibility;

	struct timespec sleepTime;

	bool m_bQuitThread;
	pthread_t m_thread;





#endif // DEVICE_STABILIZATION_HPP_INCLUDED
