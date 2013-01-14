#ifndef THREADCTRL_HPP_INCLUDED
#define THREADCTRL_HPP_INCLUDED

#include <iostream>
#include <pthread.h>
#include <signal.h>

class ThreadCtrl
{
public:
    ThreadCtrl()
    {
        m_bQuitThread = true;
    }

    virtual ~ThreadCtrl()
    {
        Stop(true);
    }

	/**
	@brief Starts the thread
	**/
	short Start()
	{
		if(m_bQuitThread)
		{
			m_bQuitThread=false;
			pthread_create(&m_thread, NULL , ThreadCtrl::ThreadWrapper, this);
		}
		else
		{
			std::cerr<<"Cannot start thread : The thread is already running"<<std::endl;
			return -1;
		}
		return 1;
	}

	/**
	@brief Stops the thread
	**/
	short Stop(bool bKill=false)
	{
		if(bKill)
		{
			pthread_kill(m_thread, 9);
			m_bQuitThread=true;
		}
		else
		{
			if(!m_bQuitThread)
			{
				m_bQuitThread=true;
			}
			else
			{
				std::cerr<<"Cannot stop thread : The thread is already stopping/stopped"<<std::endl;
				return -1;
			}
		}
		return 1;
	}

protected:
	virtual void OnThreadStart(){};
	virtual void ThreadProcess()=0;
	virtual void PostThreadProcess(){}
	virtual void OnThreadEnd(){};

	bool GetIsThreadQuitting(){return m_bQuitThread;}

private:
	static void* ThreadWrapper(void* obj)
	{
		ThreadCtrl* ctrl = reinterpret_cast<ThreadCtrl*>(obj);
		ctrl->Thread();
		return 0;
	}

	void Thread()
	{
		OnThreadStart();
		while(!m_bQuitThread)
		{
			ThreadProcess();
			PostThreadProcess();
		}
		OnThreadEnd();
	}

	bool m_bQuitThread;
	pthread_t m_thread;


};

#endif // THREADCTRL_HPP_INCLUDED
