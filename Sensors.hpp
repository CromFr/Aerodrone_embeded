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
		//ADC Pins
		for(int i=0 ; i<8 ; i++)
		{
			m_nDataPins[i] = cfg.GetValue<int>("PIN_ADC", i);
			pinMode(m_nSelectPins[i], INPUT);
		}

		//Selector pins
		m_nSelectPins[0] = cfg.GetValue<int>("PIN_Selector", 0);
		m_nSelectPins[1] = cfg.GetValue<int>("PIN_Selector", 1);
		pinMode(m_nSelectPins[0], OUTPUT);
		pinMode(m_nSelectPins[0], OUTPUT);
		digitalWrite(m_nSelectPins[0], false);
		digitalWrite(m_nSelectPins[1], false);
		m_nCurrentSelectPinsState[0] = false;
		m_nCurrentSelectPinsState[1] = false;

		selectionDelay.tv_sec=0; selectionDelay.tv_nsec=cfg.GetValue<float>("SEN_SelectionDelay")*1000.0;
	}

	/**
	@brief Get the current accelerometer value in meter.sec-2
	@note May take a little time processing (SEN_SelectionDelay)
	**/
	float GetAcceleroX()
	{
		ChangeSelection(0,0);
		m_fAccelX = GetPinsValue()/100.0;//TODO Convert byte to acceleration in m.s-2
		return m_fAccelX;
	}

	/**
	@brief Get the current accelerometer value in meter.sec-2
	@note May take a little time processing (SEN_SelectionDelay)
	**/
	float GetAcceleroY()
	{
		ChangeSelection(0,1);
		m_fAccelY = GetPinsValue()/100.0;//TODO Convert byte to acceleration in m.s-2
		return m_fAccelY;
	}

	/**
	@brief Get the current accelerometer value in meter.sec-2
	@note May take a little time processing (SEN_SelectionDelay)
	**/
	float GetAcceleroZ()
	{
		ChangeSelection(1,0);
		m_fAccelZ = GetPinsValue()/100.0;//TODO Convert byte to acceleration in m.s-2
		return m_fAccelZ;
	}

	/**
	@brief Get the current accelerometer value in meter.sec-2
	@note May take a little time processing (SEN_SelectionDelay)
	**/
	float GetGyro()
	{
		ChangeSelection(1,1);
		m_fRotSpeed = GetPinsValue()/100.0;//TODO Convert byte to rot speed in m.s-1
		return m_fRotSpeed;
	}








private:

	void ChangeSelection(bool b1, bool b0)
	{
		if(!(m_nCurrentSelectPinsState[0] == 0 && m_nCurrentSelectPinsState[1] == 0))
		{
			// @todo (crom#1#): Set select pins to 00
			nanosleep(&selectionDelay, NULL);
		}
	}

	short GetPinsValue()
	{
		short ret=0;
		for(short i=0, coeff=1 ; i<8 ; i++, coeff*=2)
			ret += coeff*digitalRead(m_nSelectPins[i]);
		return ret;
	}

	int m_nDataPins[8];
	int m_nSelectPins[2];

	bool m_nCurrentSelectPinsState[2];

	struct timespec selectionDelay;

	float m_fAccelX, m_fAccelY, m_fAccelZ, m_fRotSpeed;



};

#endif // SENSORS_HPP_INCLUDED
