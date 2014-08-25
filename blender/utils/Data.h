#ifndef _GL_DATA_OBJECTS_H_
#define _GL_DATA_OBJECTS_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include <vector>
#include "Vector3.h"
#include "Matrix4.h"

#include "config.h"

struct ShaderInfo {
	std::string name;
	GLint id;
	bool isDefined(){
		return name != "";
	}
	operator GLint(){
		return id;
	}
	virtual ~ShaderInfo() {}
};
struct Uniform : public ShaderInfo {
	Uniform(){}
	Uniform(std::string name){this->name = name;}
	int setUp(GLuint prog){
		if(name == "")return 0;
		id = glGetUniformLocation(prog, name.c_str());
		if (id == -1) {
			fprintf(stderr, "Could not bind uniform %s\n", name.c_str());
			return 0;
		}
		if(Logging::shader)
			printf("Loaded uniform %s, with id: %i\n", name.c_str(), id);	
		return 1;
	}

};
struct Attribute : public ShaderInfo {
	Attribute(){}
	Attribute(std::string name){this->name = name;}
	int setUp(GLuint prog){
		if(name == "")return 0;
		id = glGetAttribLocation(prog, name.c_str());
		if (id== -1) {
			fprintf(stderr, "Could not bind attribute %s\n", name.c_str());
			return 0;
		}
		
		if(Logging::shader)
			printf("Loaded attribute %s, with id: %i\n", name.c_str(), id);
		return 1;
	}
};


struct Color {
	GLfloat color[4];
	std::string name;
};


struct Vertex {
	Vector3 position, localPosition, worldPosition,
			normal;
	GLfloat color[4];
};
struct Face {
	GLushort indicies[3];
	Vertex *vertices[3], normal;
};

Vector3 calcNormal(Vector3 f1, Vector3 f2, Vector3 f3);

struct VertexGroup {
	Vertex *vertex;
	GLfloat intencity;
	VertexGroup():vertex(0), intencity(1.0f){}
	bool isActive(){return vertex != 0;}
};

class Bone{
public:
	std::vector<VertexGroup> vertices;
	Matrix4 rotation, translation, scale;
	Bone() {}

	void emptyPosition(){
		for (std::vector<VertexGroup>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
			it->vertex->position = Vector3();
		}
	}
	void apply(){
		for (std::vector<VertexGroup>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
			it->vertex->localPosition = it->vertex->position;
		}
	}
	void refresh(bool reset = true){
		for (std::vector<VertexGroup>::iterator it = vertices.begin(); it != vertices.end(); ++it) {
			Matrix4 boneMatrix = rotation * translation * scale;
			if(reset)
				it->vertex->position = Matrix4::mult(boneMatrix, it->vertex->localPosition);
			else
				it->vertex->position = boneMatrix * (it->vertex->position);

		}
	}
};


#endif
