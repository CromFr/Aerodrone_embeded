#ifndef SENSORS_HPP_INCLUDED
#define SENSORS_HPP_INCLUDED

class ConfigFile;
#include <cstdlib>

class Sensors
{
public:
	/**
	@brief Inits the sensors (accelerometer & gyroscope)
	**/
	Sensors(const ConfigFile* cfg);

	/**
	@brief Get the current accelerometer value in meter.sec-2
	@note May take a little time processing (SEN_SelectionDelay)
	**/
	float GetAcceleroX();

	/**
	@brief Get the current accelerometer value in meter.sec-2
	@note May take a little time processing (SEN_SelectionDelay)
	**/
	float GetAcceleroY();

	/**
	@brief Get the current accelerometer value in meter.sec-2
	@note May take a little time processing (SEN_SelectionDelay)
	**/
	float GetAcceleroZ();

	/**
	@brief Get the current accelerometer value in meter.sec-2
	@note May take a little time processing (SEN_SelectionDelay)
	**/
	float GetGyro();



private:

	void ChangeSelection(bool b1, bool b0);
	short GetPinsValue();

	int m_nDataPins[8];
	int m_nSelectPins[2];

	bool m_nCurrentSelectPinsState[2];

	struct timespec selectionDelay;

	float m_fAccelX, m_fAccelY, m_fAccelZ, m_fRotSpeed;



};

#endif // SENSORS_HPP_INCLUDED
