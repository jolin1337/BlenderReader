#include "Matrix4.h"

Matrix4::Matrix4(){
	loadIdentity();
}

Matrix4 & Matrix4::mult(Matrix4 &m2){
	float res[4][4];
	for(int r=0;r<4;++r)
		for(int c=0;c<4;++c)
			res[r][c] = matrix[0][c]*m2[r][0] + matrix[1][c]*m2[r][1] + matrix[2][c]*m2[r][2] + matrix[3][c]*m2[r][3];
	
	float *r = (float*)res, *m = (float*)matrix;
	for(int i=0;i<16;++i)
		m[i] =r[i];
	return *this;
}
Matrix4 & Matrix4::mult(int t){
	for(int r=0;r<4;r++)
		for(int c=0;c<4;c++)
			matrix[r][c] *= t;
	return *this;
}
Matrix4 & Matrix4::mult(double t){
	for(int r=0;r<4;r++)
		for(int c=0;c<4;c++)
			matrix[r][c] *= t;
	return *this;
}

Matrix4 & Matrix4::lookAt(Vector3 eye, Vector3 center, Vector3 up){
	Vector3 forward, side;
	forward = center - eye;
	forward.normalize();

	side = forward.cross(up);
	side.normalize();

	up = side.cross(forward);

	matrix[0][0] = side.x;
	matrix[1][0] = side.y;
	matrix[2][0] = side.z;

	matrix[0][1] = up.x;
	matrix[1][1] = up.y;
	matrix[2][1] = up.z;

	matrix[0][2] = -forward.x;
	matrix[1][2] = -forward.y;
	matrix[2][2] = -forward.z;

	eye.mult(-1);
	Translate(eye);

	return *this;
}
Matrix4 & Matrix4::Frustum(float left, float right, float bottom, float top, float zNear, float zFar) {
	float zDelta = (zFar-zNear);
	float dir = (right-left);
	float height = (top-bottom);
	float zNear2 = 2*zNear;

	matrix[0][0]=2.0f*zNear/dir;
	matrix[0][1]=0.0f;
	matrix[0][2]=(right+left)/dir;
	matrix[0][3]=0.0f;
	matrix[1][0]=0.0f;
	matrix[1][1]=zNear2/height;
	matrix[1][2]=(top+bottom)/height;
	matrix[1][3]=0.0f;
	matrix[2][0]=0.0f;
	matrix[2][1]=0.0f;
	matrix[2][2]=-(zFar+zNear)/zDelta;
	matrix[2][3]=-zNear2*zFar/zDelta;
	matrix[3][0]=0.0f;
	matrix[3][1]=0.0f;
	matrix[3][2]=-1.0f;
	matrix[3][3]=0.0f;
	return *this;
}
Matrix4 & Matrix4::Perspective(float fovy, float aspectRatio, float zNear, float zFar) {
	/*
	float xmin,xmax,ymin,ymax;
	ymax= zNear* tan(fovy*M_PI/360.0);
	ymin= -ymax;
	xmin= ymin*aspectRatio;
	xmax= ymax*aspectRatio;
	Frustum(xmin,xmax,ymin,ymax,zNear,zFar);
	*/
	fovy *= M_PI/180;
	float f = 1/tan(fovy/2),
		  zDelta = zNear - zFar;
	matrix[0][0] = f/aspectRatio;
	matrix[0][1] = 0;
	matrix[0][2] = 0;
	matrix[0][3] = 0;

	matrix[1][0] = 0;
	matrix[1][1] = f;
	matrix[1][2] = 0;
	matrix[1][3] = 0;

	matrix[2][0] = 0;
	matrix[2][1] = 0;
	if(zDelta != 0){
		matrix[2][2] = (zFar + zNear)/zDelta;
		matrix[2][3] = -1;
	}
	else{
		matrix[2][2] = 0;
		matrix[2][3] = 0;
	}

	matrix[3][0] = 0;
	matrix[3][1] = 0;
	matrix[3][2] = (2*zFar*zNear)/zDelta;
	matrix[3][3] = 0;
	return *this;
}

Matrix4 & Matrix4::Translate(Vector3 v){
	Matrix4 t;
	t[3][0] = v.x;
	t[3][1] = v.y;
	t[3][2] = v.z;
	mult(t);
	return *this;
}
Matrix4 & Matrix4::Rotate(float angle, Vector3 v){
	float X = angle*v.x,Y = angle*v.y,Z = angle*v.z;
	Matrix4 t;
	t[0][0] = cosf(Y) * cosf(Z);
	t[1][0] = -cosf(Y) * sinf(Z);
	t[2][0] = sinf(Y);

	t[0][1] = sinf(X) * sinf(Y) * cosf(Z) + cosf(X) * sinf(Z);
	t[1][1] = -sinf(X) * sinf(Y) * sinf(Z) + cosf(X) * cosf(Z);
	t[2][1] = -sinf(X) * cosf(Y);

	t[0][2] = -cosf(X) * sinf(Y) * cosf(Z) + sinf(X) * sinf(Z);
	t[1][2] = cosf(X) * sinf(Y) * sinf(Z) + sinf(X) * cosf(Z);
	t[2][2] = cosf(X) * cosf(Y);
	mult(t);
	return *this;
}
Matrix4 & Matrix4::Scale(Vector3 v){
	Matrix4 t;
	t[0][0] *= v.x;
	t[1][1] *= v.y;
	t[2][2] *= v.z;
	mult(t);
	return *this;
}

Matrix4 & Matrix4::loadIdentity(){
	for(int i=0; i<4;i++)
		for(int j=0;j<4;j++)
			if(j == i)
				matrix[i][j] = 1;
			else
				matrix[i][j] = 0;
	return *this;
}
float Matrix4::getValue(int r, int c){
	if(r < 0 || r > 3 || c > 3 || c < 0)
		throw "Out of bounds";
	return matrix[r][c];
}

Matrix4 Matrix4::operator *(Matrix4 m){
	return Matrix4::mult(*this, m);
}
Matrix4 Matrix4::operator *(int t){
	return Matrix4::mult(*this, t);
}
Matrix4 Matrix4::operator *(float t){
	return Matrix4::mult(*this, t);
}
Matrix4 Matrix4::operator *(double t){
	return Matrix4::mult(*this, t);
}

float *Matrix4::operator [](int row){
	if(row >3 || row < 0)
		return 0;
	return matrix[row];
}

float *Matrix4::getMatrix4(){
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			matrixOne[i*4 + j] = matrix[i][j];	
	return matrixOne;
}
void Matrix4::setMatrix4(float *m[]){
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			m[i][j] = matrixOne[i*4 + j];
}