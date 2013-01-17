#ifndef MOTOR_HPP_INCLUDED
#define MOTOR_HPP_INCLUDED

#include <WiringPiWrap.h>

/**
@brief One-motor information (pin, speed) and GPIO Wrapper
**/
class Motor
{
public:
    /**
    @arg nPin Pin where the motor is connected
    **/
    Motor(int nPin)
    {
        //Prepare and init the pin
        m_nPin = nPin;
        pinMode(nPin, OUTPUT);
        digitalWrite(nPin, 0);

        m_fSpeed = 0;
    }

    /**
    @brief Change the speed of the motor
    @param fSpeed100 Speed to set, out of 100
    **/
    void SetSpeed(float fSpeed100)
    {
        m_fSpeed = fSpeed100;
    }

    /**
    @brief Returns the current speed of the motor
    **/
    float GetSpeed()
    {
        return m_fSpeed;
    }

    /**
    @brief Change the state of the motor's pin
    **/
    void SetPin(bool bState)
    {
        digitalWrite(m_nPin, bState);
    }

private:
    int m_nPin;
    float m_fSpeed;

};

#endif // MOTOR_HPP_INCLUDED
