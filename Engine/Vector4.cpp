#include "stdafx.h"
#include "Vector3.h"
#include "Vector4.h"
#include "assert.h"
#include "math.h"

namespace Engine
{
	Vector4 Vector4::Zero = Vector4( 0, 0, 0, 0 );
	Vector4 Vector4::UnitX = Vector4( 1, 0, 0, 0 );
	Vector4 Vector4::UnitY = Vector4( 0, 1, 0, 0 );
	Vector4 Vector4::UnitZ = Vector4( 0, 0, 1, 0 );
	Vector4 Vector4::UnitW = Vector4( 0, 0, 0, 1 );

	Vector4::Vector4()
	{
	}

	Vector4::Vector4( float i_x, float i_y, float i_z, float i_w )
		: x( i_x ), y( i_y ), z( i_z ), w( i_w )
	{
	}

	Vector4::Vector4( const Vector3 & i_vct3, float i_w )
		: x( i_vct3.x ), y( i_vct3.y ), z( i_vct3.z ), w( i_w )
	{
	}

	Vector4::Vector4( const Vector4 & i_other )
		: x( i_other.x ), y( i_other.y ), z( i_other.z ), w( i_other.w )
	{
	}

	void Vector4::operator=( const Vector4 & i_other )
	{
		this->x = i_other.x;
		this->y = i_other.y;
		this->z = i_other.z;
		this->w = i_other.w;
	}

	Vector4 & Vector4::operator+( const Vector4 & i_other )
	{
		this->x += i_other.x;
		this->y += i_other.y;
		this->z += i_other.z;
		this->w += i_other.w;
		return *this;
	}

	void Vector4::operator+=( const Vector4 & i_other )
	{
		this->x += i_other.x;
		this->y += i_other.y;
		this->z += i_other.z;
		this->w += i_other.w;
	}

	Vector4 & Vector4::operator-( const Vector4 & i_other )
	{
		this->x -= i_other.x;
		this->y -= i_other.y;
		this->z -= i_other.z;
		this->w -= i_other.w;
		return *this;
	}

	void Vector4::operator-=( const Vector4 & i_other )
	{
		this->x -= i_other.x;
		this->y -= i_other.y;
		this->z -= i_other.z;
		this->w -= i_other.w;
	}

	Vector4 & Vector4::operator*( float value )
	{
		this->x *= value;
		this->y *= value;
		this->z *= value;
		this->w *= value;
		return *this;
	}

	void Vector4::operator*=( float value )
	{
		this->x *= value;
		this->y *= value;
		this->z *= value;
		this->w *= value;
	}

	Vector4 & Vector4::operator/( float value )
	{
		assert( value != 0 );
		this->x /= value;
		this->y /= value;
		this->z /= value;
		this->w /= value;
		return *this;
	}

	void Vector4::operator/=( float value )
	{
		assert( value != 0 );
		this->x /= value;
		this->y /= value;
		this->z /= value;
		this->w /= value;
	}

	float Vector4::Length()
	{
		return sqrt( this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w );
	}

	Vector4::~Vector4()
	{
	}
}