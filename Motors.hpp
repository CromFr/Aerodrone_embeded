#ifndef MOTORS_HPP_INCLUDED
#define MOTORS_HPP_INCLUDED

#include <iostream>
#include <sys/time.h>

class ConfigFile;


class Motors
{
private:
	class Motor
	{
	public:
		Motor(int nPin);

		/**
		@brief Change the speed of the motor
		@param fSpeed100 Speed to set, out of 100
		**/
		void SetSpeed(float fSpeed100);

		/**
		@brief Returns the current speed of the motor
		**/
		float GetSpeed();

		int GetPin();

	private:
		int m_nPin;
		float m_fSpeed;

	};

public:
	/**
	@brief Motors handler initialisation
	**/
	Motors(const ConfigFile* cfg);
	~Motors();


	/**
	@brief Endless loop function to generate the PWM (to be executed in a separated thread)
	**/
	void PWMThread();

	/**

	**/
	void StartThread();

	/**
	@brief Stops the thread (quit endless loop)
	**/
	void QuitThread();


	void SetSpeed(int motID, float fPercent);

	float GetSpeed(int motID);

private:
	Motor* m_mot[4];
	float m_fMotorMinSpeed;
	float m_fProcessPrecision;

	float m_fPWMFreq;

	static void* MotorsThreadWrapper(void* obj);
	bool m_bQuitThread;
	pthread_t m_thread;






};



#endif // MOTORS_HPP_INCLUDED
