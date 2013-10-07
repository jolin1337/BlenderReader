#ifndef _MATRIX4x4_H_
#define _MATRIX4x4_H_ 

#include "Vector3.h"
#include <math.h>

class Matrix4 {
	float matrixOne[16];
	float matrix[4][4];
public:
	Matrix4();

	Matrix4 & mult(Matrix4 &m2);
	Matrix4 & mult(int t);
	Matrix4 & mult(double t);
	Matrix4 & Translate(Vector3 v);
	Matrix4 & Rotate(float angle, Vector3 v);
	Matrix4 & RotateA(float angle, Vector3 v){
		angle = angle * M_PI/180;
		return Rotate(angle, v);
	}
	Matrix4 & Scale(Vector3 v);
	Matrix4 & loadIdentity();
	Matrix4 & lookAt(Vector3 eye, Vector3 center, Vector3 up);

	Matrix4 & Frustum(float left, float right, float bottom, float top, float zNear, float zFar);
	Matrix4 & Perspective(float fovy,float aspectRatio,float zNear,float zFar);

	float getValue(int x, int y);
	void getMatrix4(float m[4][4]){
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				m[i][j] = matrix[i][j];
	}
	float *getMatrix4();
	void setMatrix4(float *m[]);
	float *operator [](int row);
	Matrix4 operator *(Matrix4 m);
	Matrix4 operator *(int t);
	Matrix4 operator *(float t);
	Matrix4 operator *(double t);
	Vector3 operator *(Vector3 v){
		return Matrix4::mult(*this, v);
	}

	operator float *(){
		return (float *)(&(matrix[0]));
	}

	static Matrix4 mult(Matrix4 m1, Matrix4 m2){
		return m1.mult(m2);
	}
	static Vector3 mult(Matrix4 m, Vector3 v){
		Vector3 res;
		res.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[3][0];
		res.y = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[3][1];
		res.z = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[3][2];
		return res;
	}
	static Matrix4 mult(Matrix4 m1, float f){
		return m1.mult(f);
	}
};

#endif
