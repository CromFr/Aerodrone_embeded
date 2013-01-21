#ifndef LIVINGTHREAD_HPP_INCLUDED
#define LIVINGTHREAD_HPP_INCLUDED

namespace std{class thread;}

/**
@brief Pure virtual class to define objects with threads running on
@note You must re-write "void ThreadProcess()"
**/
class LivingThread
{
public:
    LivingThread(const char* sName="");
    virtual ~LivingThread();

	/**
	@brief Starts the thread
	@return true is the thread was correctly started, false if the thread was already running
	**/
	bool Start();

	/**
	@brief Waits until the thread is runing
	@arg nTimeoutS timeout in seconds until we stop waiting
	@return false if the thread is not running when the timeout expires, true if the thread is running
	**/
	bool WaitUntilStarted(float nTimeoutS);

	/**
	@brief Stops the thread
	@arg bKill If true, it will instantanly stop the thread. Not recommended
	@return true is the thread was correctly stopped, false if the thread was already stopped
	**/
	bool Stop(bool bKill=false);

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
	static void ThreadWrapper(void* obj);

	void ThreadFunction();

    bool m_bThreadRunning;
	bool m_bQuitThread;
	std::thread* m_thread;
	const char* m_sName;


};

#endif // LIVINGTHREAD_HPP_INCLUDED
