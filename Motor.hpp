#ifndef MOTOR_HPP_INCLUDED
#define MOTOR_HPP_INCLUDED

#include <WiringPi.h>

class Motor
{
public:
    Motor(int nPin)
    {
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

    void SetPin(bool bState)
    {
        digitalWrite(m_nPin, bState);
    }

private:
    int m_nPin;
    float m_fSpeed;

};

#endif // MOTOR_HPP_INCLUDED
