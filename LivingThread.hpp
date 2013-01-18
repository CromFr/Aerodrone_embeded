#ifndef LIVINGTHREAD_HPP_INCLUDED
#define LIVINGTHREAD_HPP_INCLUDED

#include <iostream>
#include <thread>

/**
@brief Pure virtual class to define objects with threads running on
@note You must re-write "void ThreadProcess()"
**/
class LivingThread
{
public:
    LivingThread()
    {
        m_bQuitThread = true;
        m_bThreadRunning = false;
    }
    virtual ~LivingThread()
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
			//m_thread = new std::thread(ThreadWrapper, this);//TODO uncomment
			m_thread = new std::thread(&ThreadWrapper, this);//TODO uncomment
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
	@arg bKill If true, it will instantanly stop the thread. Not recommended
	**/
	short Stop(bool bKill=false)
	{
	    if(bKill)
	    {
            m_thread->detach();
            delete m_thread;
			OnThreadEnd();
            m_bThreadRunning = false;
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

	/**
	@brief Gets if the current thread is processing
	@return true if the thread is running
	**/
	bool GetIsRunning(){return m_bThreadRunning;}

protected:
	/**
	@brief This function will be called when the thread is started
	**/
	virtual void OnThreadStart(){};

	/**
	@brief The thread function you must re-write, and that will be called infinitely until the thread stops
	@warning you shouldn't put a while(1) into
	**/
	virtual void ThreadProcess()=0;

	/**
	@brief This function will be called when the thread is stopped
	@note even when Stop(true)
	**/
	virtual void OnThreadEnd(){};

	/**
	@brief Gets if the current thread is quitting (Quit(false) called) or quitted
	@return true if the thread is quitting/not running
	@note Must be put in any blocking function/loop to check if the thread must stop or can continue processing
	**/
	bool GetIsThreadQuitting(){return m_bQuitThread;}

private:
	static void ThreadWrapper(void* obj)
	{
		LivingThread* ctrl = reinterpret_cast<LivingThread*>(obj);
		ctrl->ThreadFunction();
	}

	void ThreadFunction()
	{
	    m_bThreadRunning = true;
		OnThreadStart();
		while(!m_bQuitThread)
		{
			ThreadProcess();
		}
		OnThreadEnd();
		m_bThreadRunning = false;
	}

    bool m_bThreadRunning;
	bool m_bQuitThread;
	std::thread* m_thread;


};

#endif // LIVINGTHREAD_HPP_INCLUDED
