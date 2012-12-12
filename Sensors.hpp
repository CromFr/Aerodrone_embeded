#ifndef SENSORS_HPP_INCLUDED
#define SENSORS_HPP_INCLUDED

#include "ConfigFile.hpp"

class Sensors
{
public:
	/**
	@brief Inits the sensors (accelerometer & gyroscope)
	**/
	Sensors(const ConfigFile& cfg)
	{
		for(int i=0 ; i<8 ; i++)
			m_nDataPins[i] = cfg.GetValue<int>("PIN_ADC", i);

		m_nSelectPins[0] = cfg.GetValue<int>("PIN_Selector", 0);
		m_nSelectPins[1] = cfg.GetValue<int>("PIN_Selector", 1);

		selectionDelay.tv_sec=0; selectionDelay.tv_nsec=cfg.GetValue<float>("SEN_SelectionDelay")*1000.0;

		m_nCurrentSelectPinsState[0] = false;
		m_nCurrentSelectPinsState[1] = false;

		// @todo (crom#1#): Set select pins to 0 & set pin states to 0
	}

	/**
	@brief Get the current accelerometer value in meter.sec-2
	@note May take a little time processing (SEN_SelectionDelay)
	**/
	double GetAcceleroX()
	{
		ChangeSelection(0,0);
		int nValue;//The byte of the pins
		m_fAccelX = 0.0;
		return m_fAccelX;
	}

	//...........

	//TODO Implement this
	double GetAcceleroY(){return 0;}
	double GetAcceleroZ(){return 0;}
	double GetGyro(){return 0;}







private:

	void ChangeSelection(bool b1, bool b0)
	{
		if(!(m_nCurrentSelectPinsState[0] == 0 && m_nCurrentSelectPinsState[1] == 0))
		{
			// @todo (crom#1#): Set select pins to 00
			nanosleep(&selectionDelay, NULL);// @note (crom#1#): SegFault?
		}
	}

	int m_nDataPins[8];
	int m_nSelectPins[2];

	bool m_nCurrentSelectPinsState[2];

	struct timespec selectionDelay;

	float m_fAccelX, m_fAccelY, m_fAccelZ;



};

#endif // SENSORS_HPP_INCLUDED
