#include "Data.h"

Vector3 calcNormal(Vector3 f1, Vector3 f2, Vector3 f3){
	Vector3 v1 = f1 - f2,
			v2 = f1 - f3;
	return v1.cross(v2);
}