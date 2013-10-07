#ifndef GL_OBJECT_H
#define GL_OBJECT_H

#include "Data.h"

#include <fstream>

typedef std::vector<Vertex> Vertices;
typedef std::vector<GLushort> Faces;

typedef std::vector<Uniform> Uniforms;
typedef std::vector<Attribute> Attributes;

typedef GLuint BufferObject;

class GLobject {
public:
	friend void idle();
	friend int init_resources();

	GLobject();
	~GLobject();

	Matrix4 getTransformMatrix();

	GLobject & setPosition(Vector3 v);
	Vector3 getPosition() const;

	GLobject & setRotation(Vector3 r);
	Vector3 getRotation() const;

	GLobject & setScale(Vector3 s);
	Vector3 getScale() const;
	
	std::vector<Color > loadMaterials(std::string fileName);
	int loadShaders(std::string fileName_v, std::string fileName_f);
	void loadObj(std::string fileName);
	void render();
	
	int setupUniforms();
	int setupAttributes();
	
	void updateElement();
	void updateElement(GLuint from, GLuint size);
	void updateElement(GLuint element);

	GLuint getVboVerticesArray() const;
	GLuint getVboTextureArray() const;
	GLuint getIboIndiciesArray() const;
	
	bool isModel();
	GLfloat dimension();
	
	void addAttribute(Attribute a);
	Attribute getAttribute(GLuint a) const;
	Attribute getAttribute(std::string name) const;
	
	void addUniform(Uniform u);
	Uniform getUniform(GLuint u) const;
	Uniform getUniform(std::string name) const;
	
	GLuint getProgram() const;

	
	Vertex *getVertices();
	GLushort *getFaces();
	
	GLuint f_size();
	GLuint size();
	void setupBuffers();

	Vector3 max, min;
protected:
	Vector3 position, rotation, scale;
	Vertices vertices;
	Faces faces;
	BufferObject vbo_vertices, ibo_indicies, vbo_textureCoordinates;
	
	Uniforms uniforms;
	Attributes attributes;
	GLuint prog;

	bool _allokatedMem,
		 linkedShaders;
};

#endif