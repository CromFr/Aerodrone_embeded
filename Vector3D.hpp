#ifndef VECTOR3D_HPP_INCLUDED
#define VECTOR3D_HPP_INCLUDED

#include <cmath>

template<typename T>
class Vector3D
{
public:
	Vector3D(T _x=0, T _y=0, T _z=0){x=_x; y=_y; z=_z;}

	T x;
	T y;
	T z;

	float PlanGetZAt(T _x, T _y)
	{
		return x*_x+y*_y;
	}

	Vector3D& Normalize()
	{
		T sqlength = x*x+y*y+z*z;
		T length = pow(sqlength, 1/3.f);
		x/=length;
		y/=length;
		z/=length;
		return *this;
	}

private:


};

#endif // VECTOR3D_HPP_INCLUDED
