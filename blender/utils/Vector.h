#ifndef VECTOR_BASE_H
#define VECTOR_BASE_H

#include <math.h>

class Vector{
public:
	float x,y;
	Vector():x(0),y(0){}
	Vector(float p1,float p2){this->x=p1;this->y=p2;}
	Vector(float p1){this->x=p1;this->y=0;}
	Vector &operator ++(){
		this->x++;
		this->y++;
		return *this;
	}
	Vector &operator --(){
		this->x--;
		this->y--;
		return *this;
	}
	Vector operator +(Vector v){
		return add(*this, v);
	}
	Vector operator -(Vector v){
		return sub(*this, v);
	}
	Vector operator *(Vector v){
		return mult(*this, v);
	}
	Vector operator /(Vector v){
		return div(*this, v);
	}
	float &operator [](int index){
		switch(index){
			case 0:
				return x;
			case 1:
				return y;
			default:
				throw "Vector3.h: index out of range";
		}
	}
	float operator [](int index) const{
		switch(index){
			case 0:
				return x;
			case 1:
				return y;
			default:
				throw "Vector3.h: index out of range";
		}
	}

	float norm(){
		return sqrt(x*x + y*y);
	}
	float length(){
		return norm();
	}
	float dot(Vector &v){
		return v.x*x + v.y*y;
	}
	Vector &add(Vector &v){
		x += v.x;
		y += v.y;
		return *this;
	}
	Vector &add(float s){
		x += s;
		y += s;
		return *this;
	}
	Vector &sub(Vector &v){
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vector &sub(float s){
		x -= s;
		y -= s;
		return *this;
	}
	Vector &mult(Vector &v){
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vector &mult(float s){
		x *= s;
		y *= s;
		return *this;
	}
	Vector &div(Vector &v){
		x /= v.x;
		y /= v.y;
		return *this;
	}
	Vector &div(float s){
		x /= s;
		y /= s;
		return *this;
	}

	Vector &invert(){
		x = 1/x;
		y = 1/y;
		return *this;
	}
	float distanceTo(Vector v3){
		Vector dthis = sub(*this, v3);
		return sqrt(dthis.dot(dthis));
	}
	Vector & normalize(){
		float m = this->norm();
		if( m > 0 )
			this->div(m);
		return *this;
	}
	Vector getUnit(){
		float m = this->norm();
		if( m > 0 )
			return div(*this,m);
		else
			return *this;
		return *this;
	}


	static Vector add(Vector v1, Vector v2){
		float x = v1.x + v2.x,
			   y = v1.y + v2.y;
		return Vector(x,y);
	}
	static Vector add(Vector v1, float s){
		float x = v1.x + s,
			   y = v1.y + s;
		return Vector(x,y);
	}
	static Vector sub(Vector v1, Vector v2){
		float x = v1.x - v2.x,
			   y = v1.y - v2.y;
		return Vector(x,y);
	}
	static Vector sub(Vector v1, float s){
		float x = v1.x - s,
			   y = v1.y - s;
		return Vector(x,y);
	}
	static Vector mult(Vector v1, Vector v2){
		float x = v1.x * v2.x,
			   y = v1.y * v2.y;
		return Vector(x,y);
	}
	static Vector mult(Vector v1, float s){
		float x = v1.x * s,
			   y = v1.y * s;
		return Vector(x,y);
	}
	static Vector div(Vector v1, Vector v2){
		float x = v1.x / v2.x,
			   y = v1.y / v2.y;
		return Vector(x,y);
	}
	static Vector div(Vector v1, float s){
		float x = v1.x / s,
			   y = v1.y / s;
		return Vector(x,y);
	}
};

#endif