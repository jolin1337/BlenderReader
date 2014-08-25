#ifndef GLMESH_H
#define GLMESH_H
#include <Blender.h>
#include <utils/GLobject.h>
#include <AnimData.h>
#include <exception>

typedef Blender::Vertex Vertex_cp;

class GLMesh : public Blender::Mesh{
	// static const char ** f_shader;
	// static const char ** v_shader;
public:
	GLMesh();
	virtual ~GLMesh();
	GLMesh &operator=(const Blender::Mesh &mesh);
	GLMesh &operator=(const GLMesh &mesh);
	GLMesh(const GLMesh &mesh);
	GLMesh(const Blender::Mesh &mesh);
	void initOPGL();
	void setupBuffers();

	GLuint getProgram() const;
	int setupUniforms();
	int setupAttributes();

	int loadShaders(std::string fileName_v, std::string fileName_f);
	void render();
	void updateElement();
	void updateElement(GLuint from, GLuint size);
	void updateElement(GLuint element);
	GLuint getVboVerticesArray() const;
	GLuint getVboTextureArray() const;
	GLuint getIboIndiciesArray() const;
	bool isModel();
	void addAttribute(Attribute a);
	Attribute getAttribute(GLuint a) const;
	Attribute getAttribute(std::string name) const;
	Uniform getUniform(GLuint u) const;
	Uniform getUniform(std::string name) const;
	void addUniform(Uniform u);
	Vector3 max, min;
private:
	// Blender::AnimData adr;
	
protected:
	GLobject *parent;
	// Vector3 scale;
	// Vector3 position, rotation;
	// Vertices vertices;
	// Faces faces;
	// GLfloat *col;
	BufferObject vbo_textureCoordinates;
	BufferObject vbo_vertices, ibo_indicies;
	
	Uniforms uniforms;
	Attributes attributes;
	GLuint prog;

	bool _allokatedMem,
		 linkedShaders;
};

#endif 