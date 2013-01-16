#ifndef VECTOR3D_HPP_INCLUDED
#define VECTOR3D_HPP_INCLUDED
#include <iostream>
#include <cmath>

template<typename T>
class Vector3D
{
public:
	Vector3D(const T& _x=0, const T& _y=0, const T& _z=0){x=_x; y=_y; z=_z;}
	Vector3D(const Vector3D<T>& v){x=v.x; y=v.y; z=v.z;}

	void Set(T _x, T _y, T _z){this->x=_x; this->y=_y; this->z=_z;}

	T x;
	T y;
	T z;

	T PlanGetZAt(const T& _x, const T& _y)
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

	T GetLength()const
	{
		return sqrt(GetSQLength());
	}

	T GetSQLength()const
	{
		return x*x+y*y+z*z;
	}

	Vector3D<T> operator+(const Vector3D<T>& other){return Vector3D<T>(this->x+other.x, this->y+other.y, this->z+other.z);}
	void operator+=(const Vector3D<T>& other){this->x+=other.x; this->y+=other.y; this->z+=other.z;}

	Vector3D<T> operator-(const Vector3D<T>& other){return Vector3D<T>(this->x-other.x, this->y-other.y, this->z-other.z);}
	void operator-=(const Vector3D<T>& other){this->x-=other.x; this->y-=other.y; this->z-=other.z;}

	Vector3D<T> operator*(const T& other){return Vector3D<T>(this->x*other, this->y*other, this->z*other);}
	Vector3D<T> operator*=(const T& other){this->x*=other; this->y*=other; this->z*=other;}

	Vector3D<T> operator/(const T& other){return (*this)*(1/other);}
	Vector3D<T> operator/=(const T& other){(*this)*=(1/other);}

private:


};

#endif // VECTOR3D_HPP_INCLUDED
