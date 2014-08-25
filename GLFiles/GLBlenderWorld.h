#ifndef GLBLENDERWORLD
#define GLBLENDERWORLD
#include <Blender.h>
#include <utils/GLobject.h>
#include <AnimData.h>
#include <exception>

class GLBlender : public GLobject {
	Blender::AnimData adr;
public:
	GLBlender():GLobject() {
		vbo_vertices = -1;
		ibo_indicies = -1;
	}
	int loadMesh(Blender::Mesh *mesh);
	int getFrameCount(){
		return adr.getFrameCount();
	}
	bool setFrame(int frame);
};


#endif