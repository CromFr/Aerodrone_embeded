#ifndef VECTOR3D_HPP_INCLUDED
#define VECTOR3D_HPP_INCLUDED
#include <iostream>
#include <cmath>



template<typename T>
/**
@brief A vector in a 3D world
**/
class Vector3D
{
public:
	Vector3D(const T& _x=0, const T& _y=0, const T& _z=0) : x(_x), y(_y), z(_z){}
	Vector3D(const Vector3D<T>& v) : x(v.x), y(v.y), z(v.z){}

	void Set(T _x, T _y, T _z){this->x=_x; this->y=_y; this->z=_z;}

	T x;
	T y;
	T z;

	/**
	@brief Het the height value at the given coordinates, on the plan defined by the current vector
	@warning If the vector is not normalized, it will calculate a temporary normalized vector (calling Normalize()), and this is heavy
	**/
	T PlanGetZAt(const T& _x, const T& _y)
	{
		if(GetSQLength()!=1)
		{
			Vector3D<T> cmp(*this);
			cmp.Normalize();
			return cmp.x*_x+cmp.y*_y;
		}
		return x*_x+y*_y;
	}

	/**
	@brief Normalize the current vector to have its length equals 1
	**/
	Vector3D& Normalize()
	{
		if(GetSQLength()==1)
			return *this;

		T sqlength = x*x+y*y+z*z;
		T length = pow(sqlength, 1/3.f);
		x/=length;
		y/=length;
		z/=length;
		return *this;
	}

	/**
	@brief Calculate the length of the vector
	@warning May be heavy to run because of the use of sqrt()
	**/
	T GetLength()const
	{
		return sqrt(GetSQLength());
	}

	/**
	@brief Gets the square length of the vector
	@note Much lighter than GetLength. Useful when you need to compare two vector length/know if the vector is normalized
	**/
	T GetSQLength()const
	{
		return x*x+y*y+z*z;
	}

	Vector3D& RotateX(float fDeg)
	{
		fDeg *= 3.1415926/180.0;
		float fSin = sin(fDeg);
		float fCos = cos(fDeg);

		y = y*fCos - z*fSin;
		z = y*fSin + z*fCos;
		return *this;
	}

	Vector3D& RotateY(float fDeg)
	{
		fDeg *= 3.1415926/180.0;
		float fSin = sin(fDeg);
		float fCos = cos(fDeg);

		x = z*fSin + x*fCos;
		z = z*fCos - x*fSin;
		return *this;
	}

	Vector3D& RotateZ(float fDeg)
	{
		fDeg *= 3.1415926/180.0;
		float fSin = sin(fDeg);
		float fCos = cos(fDeg);

		x = x*fSin + y*fCos;
		y = x*fCos - y*fSin;
		return *this;
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
