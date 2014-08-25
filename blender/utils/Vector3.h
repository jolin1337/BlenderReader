#ifndef _VECTOR3_
#define _VECTOR3_

#include "Vector.h"
#include <math.h>
#include <string>
class Vector3 {
public:
	Vector3():z(0){}
	Vector3(float p1,float p2,float p3){this->x=p1;this->y=p2;this->z=p3;}
	Vector3(float p1,float p2){this->x=p1;this->y=p2;this->z=0;}
	Vector3(float p1){this->x=p1;this->y=0;this->z=0;}
	Vector3(Vector *c){this->x=c->x;this->y=c->y;this->z=0;}
	Vector3(Vector3 *c){this->x=c->x;this->y=c->y;this->z=c->z;}
	Vector3 & operator ++(){
		return add(1,1,1);
	}
	Vector3 & operator --(){
		return sub(1,1,1);
	}
	Vector3 operator *(float k){
		return Vector3::mult(this, k);
	}
	Vector3 operator +(float s){
		return Vector3(x+s, y+s, z+s);
	}
	Vector3 operator +(Vector3 s){
		return Vector3::add(this, &s);
	}
	Vector3 operator -(float s){
		return Vector3(x-s, y-s, z-s);
	}
	Vector3 operator -(Vector3 s){
		return Vector3::sub(this, &s);
	}
	float &operator [](int index){
		switch(index){
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
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
			case 2:
				return z;
			default:
				throw "Vector3.h: index out of range";
		}
	}

	Vector3 get(){return Vector3(this->x,this->y,this->z);}

	float norm(){return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);}

	Vector3 & add(float p1,float p2,float p3){this->x += p1;this->y += p2;this->z += p3;return *this;}
	Vector3 & add(Vector3 *v3){this->x += v3->x;this->y += v3->y;this->z += v3->z;return *this;}
	Vector3 & add(Vector3 v3){return add(&v3);}

	Vector3 & sub(float p1,float p2,float p3){return add(-p1,-p2,-p3);}
	Vector3 & sub(Vector3 *v3){return add(-v3->x, -v3->y, -v3->z);}

	Vector3 & mult(Vector3 *v3){this->x *= v3->x;this->y *= v3->y;this->z *= v3->z;return *this;}
	Vector3 & mult(float s){this->x *= s;this->y *= s;this->z *= s;return *this;}

	Vector3 & div(Vector3 *v3){this->x /= v3->x;this->y /= v3->y;this->z /= v3->z;return *this;}
	Vector3 & div(float s){return this->mult(1/s);}

	float distance(Vector3 *v3){
		Vector3 dthis = Vector3::sub(this, v3);
		return sqrt(dthis.dot(&dthis));
	}
	float dot(float v1,float v2,float v3){
		return (this->x*v1 + this->y*v2 + this->z*v3);
	}
	float dot(Vector3 *v){
		return (this->x*v->x + this->y*v->y + this->z*v->z);
	}
	Vector3 cross(Vector3 v){
		return Vector3(
			this->y*v.z - v.y*this->z,
			this->z*v.x - v.z*this->x,
			this->x*v.y - v.x*this->y
		);
	}
	Vector3 & normalize(){
		float m = this->norm();
		if( m > 0 )
			this->div(m);
		return *this;
	}
	Vector3 & limit(float highV){
		if(this->norm() > highV){
			this->normalize();
			this->mult(highV);
		}
		return *this;
	}
	/*const char * toString(){
		return "[ "+x+", "+y+", "+z+" ]";
	}*/
	static Vector3 add(Vector3 *v,Vector3 *w){
		return Vector3((v->x + w->x), (v->y + w->y), (v->z + w->z));
	}
	static Vector3 sub(Vector3 *v,Vector3 *w){
		return Vector3((v->x - w->x), (v->y - w->y), (v->z - w->z));
	}
	static float dot(Vector3 *v,Vector3 *w){
		return (w->x*v->x + w->y*v->y + w->z*v->z);
	}
	static Vector3 mult(Vector3 *v,float w){
		return Vector3(v->x * w, v->y * w, v->z * w);
	}
	float x,y,z;
};
#endif
