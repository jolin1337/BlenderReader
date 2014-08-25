#include "GLArmature.h"

GLArmature::GLArmature():vbo_vertices(-1), ibo_indicies(-1), vbo_textureCoordinates(-1), 
				 prog(-1), _allokatedMem(false), linkedShaders(false) {
}

GLArmature::~GLArmature() {
	if((int)prog != -1)
		glDeleteProgram(prog);
	if((int)vbo_vertices != -1)
		glDeleteBuffers(1, &vbo_vertices);
	if((int)ibo_indicies != -1)
		glDeleteBuffers(1, &ibo_indicies);

	if((int)vbo_textureCoordinates != -1)
		glDeleteBuffers(1, &vbo_textureCoordinates);
}

GLArmature::GLArmature(const Blender::Armature &arm):Blender::Armature(arm), 
				 vbo_vertices(-1), ibo_indicies(-1), vbo_textureCoordinates(-1), 
				 prog(-1), _allokatedMem(false), linkedShaders(false) {
	// initOPGL();
}
GLArmature::GLArmature(const GLArmature &arm):Blender::Armature(arm), vbo_vertices(-1), ibo_indicies(-1), vbo_textureCoordinates(-1), 
				 prog(-1), _allokatedMem(false), linkedShaders(false) {
	throw "Not implemented";
	initOPGL();
}

GLArmature &GLArmature::operator=(const Blender::Armature &arm) {
	throw "Not implemented";
	Blender::Armature::operator=(arm);
	initOPGL();
}

GLArmature &GLArmature::operator=(const GLArmature &arm) {
	throw "Not implemented";
	Blender::Armature::operator=(arm);
	initOPGL();
}


void GLArmature::initOPGL() {
	if((int)prog != -1)
		glDeleteProgram(prog);
	if((int)vbo_vertices != -1)
		glDeleteBuffers(1, &vbo_vertices);
	if((int)ibo_indicies != -1)
		glDeleteBuffers(1, &ibo_indicies);

	if((int)vbo_textureCoordinates != -1)
		glDeleteBuffers(1, &vbo_textureCoordinates);

	setupBuffers();

	// uniform that will be used in the shader 
	Uniform u("m_transform");   
	Attribute a1("position"),   // position that will represent all the vercies
			  // a2("texcoord"), // textures uppcomming TODOS
			  a3("color"),      // color of the object that was loaded
			  a4("normal");     // normal attribute

	// Add all the uniforms and attributes for this key-object 
	addUniform(u);
	addAttribute(a1);
	addAttribute(a3);
	addAttribute(a4);
	
	// load vertex-shader and fragment-shader to a program and bind to our properties
	int status = loadShaders("GLFiles/shaders/wall.v-shader", "GLFiles/shaders/wall.f-shader");
	if(status != 1) // if loadshaders failed to load
		throw status; // abort
}

void GLArmature::setupBuffers() {
	float len = 3.0f;
	Blender::Material m;
	m.r = m.g = m.b = 0.0f;
	m.ref = 0.3f;
	m.name="Default_arm_color";

	std::vector<Vertex_cp> verts;

	float zlen = 0.9f, zblen = 0.9f;
	float ylen = 0.9f, yblen = 0.9f;
	float xlen = 0.9f, xblen = 0.9f;
	Vertex_cp vert;
	vert.mat_nr = 0;
	vert.mat = m;
	vert.position = Vector3(-xblen, -ylen, -zblen);	// 0
	verts.push_back(vert);
	vert.position = Vector3(-xblen, yblen, -zblen);	// 1
	verts.push_back(vert);
	vert.position = Vector3(xlen, yblen, -zblen);		// 2
	verts.push_back(vert);
	vert.position = Vector3(xlen, -ylen, -zblen);	// 3
	verts.push_back(vert);

	vert.position = Vector3(-xblen, yblen, zlen);	// 4
	verts.push_back(vert);
	vert.position = Vector3(-xblen, -ylen, zlen);	// 5
	verts.push_back(vert);
	vert.position = Vector3(xlen, -ylen, zlen);	// 6
	verts.push_back(vert);
	vert.position = Vector3(xlen, yblen, zlen);		// 7
	verts.push_back(vert);

	// for (std::vector<Vertex_cp>::iterator it = verts.begin(); it != verts.end(); ++it) {
	// 	it->position.add(Vector3(-xblen/2,0,0));
	// }

	glGenBuffers(1, &ibo_indicies);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_indicies);
	
	int *indicies_buffer = new int[36];
	//begin square
	indicies_buffer[0] = 0;
	indicies_buffer[1] = 1;
	indicies_buffer[2] = 2;

	indicies_buffer[3] = 0;
	indicies_buffer[4] = 2;
	indicies_buffer[5] = 3;
	// end square
	//begin square
	indicies_buffer[6] = 0;
	indicies_buffer[7] = 5;
	indicies_buffer[8] = 4;

	indicies_buffer[9] = 0;
	indicies_buffer[10] = 4;
	indicies_buffer[11] = 1;
	// end square
	//begin square
	indicies_buffer[12] = 1;
	indicies_buffer[13] = 7;
	indicies_buffer[14] = 2;

	indicies_buffer[15] = 1;
	indicies_buffer[16] = 4;
	indicies_buffer[17] = 7;
	// end square
	//begin square
	indicies_buffer[18] = 2;
	indicies_buffer[19] = 7;
	indicies_buffer[20] = 6;

	indicies_buffer[21] = 2;
	indicies_buffer[22] = 6;
	indicies_buffer[23] = 3;
	// end square
	//begin square
	indicies_buffer[24] = 0;
	indicies_buffer[25] = 3;
	indicies_buffer[26] = 5;

	indicies_buffer[27] = 3;
	indicies_buffer[28] = 6;
	indicies_buffer[29] = 5;
	// end square
	//begin square
	indicies_buffer[30] = 5;
	indicies_buffer[31] = 6;
	indicies_buffer[32] = 7;

	indicies_buffer[33] = 5;
	indicies_buffer[34] = 7;
	indicies_buffer[35] = 4;
	// end square

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(int), indicies_buffer, GL_STATIC_DRAW);

	delete[] indicies_buffer;
	
	glGenBuffers(1, &(vbo_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(Vertex_cp), &verts[0], GL_DYNAMIC_DRAW);
}


GLuint GLArmature::getProgram() const {
	return prog;
}
int GLArmature::setupUniforms() {
	if(!linkedShaders)return 0;
	int res = 1;
	for(Uniforms::iterator it = uniforms.begin(); it != uniforms.end(); it++)
		if((*it).setUp(prog) == 0)
			res = 0;
	return res;
}
int GLArmature::setupAttributes() {
	if(!linkedShaders)return 0;
	int res = 1;
	for(Attributes::iterator it = attributes.begin(); it != attributes.end(); it++)
		if((*it).setUp(prog) == 0)
			res = 0;
	return res;
}

int GLArmature::loadShaders(std::string fileName_v, std::string fileName_f) {
	Uniform otr = getUniform("o_transform");
	if(!otr.isDefined())
		addUniform(Uniform("o_transform"));

	GLint link_ok = GL_FALSE;
	GLuint vs, fs;
	if ((vs = create_shader(fileName_v.c_str(), GL_VERTEX_SHADER))   == 0) return 0;
	if ((fs = create_shader(fileName_f.c_str(), GL_FRAGMENT_SHADER)) == 0) return 0;

	prog = glCreateProgram();

	glAttachShader(prog, vs);
	glAttachShader(prog, fs);

	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		linkedShaders = false;
		fprintf(stderr, "glLinkProgram:\n");
		return 0;
	}
	linkedShaders = true;

	if(this->setupUniforms() + this->setupAttributes() == 0)
		return 0;

	return 1;
}

void GLArmature::renderOnce() {
	Attribute coord = getAttribute("position"),
			  color = getAttribute("color"),
			  normal = getAttribute("normal");
	bool useAttr = coord.isDefined(), 
		useColor = color.isDefined() && color.name != coord.name,
		useNorm = normal.isDefined() && normal.name != color.name && normal.name != coord.name;
	if(useAttr)
		glEnableVertexAttribArray(coord);
	if(useColor)
		glEnableVertexAttribArray(color);
	if(useNorm)
		glEnableVertexAttribArray(normal);

	glBindBuffer(GL_ARRAY_BUFFER, getVboVerticesArray());
		int size;
		// glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		if(useAttr)
			glVertexAttribPointer(
				coord, 
				3, 
				GL_FLOAT, 
				GL_FALSE, 
				sizeof(Vertex_cp),
				(void*)(offsetof(Vertex_cp,position))
			);
		if(useColor)
			glVertexAttribPointer(
				color, 
				4, 
				GL_FLOAT, 
				GL_FALSE, 
				sizeof(Vertex_cp),
				(void*)offsetof(Vertex_cp,mat.r)
			);
		if(useNorm)
			glVertexAttribPointer(
				normal, 
				3, 
				GL_FLOAT, 
				GL_FALSE, 
				sizeof(Vertex_cp),
				(void*)(offsetof(Vertex_cp,normal))
			);
		// glColorPointer(4, GL_FLOAT, 0, getVertices());
		glNormalPointer(GL_FLOAT, sizeof(Vertex_cp), (void*)(offsetof(Vertex_cp,normal)));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getIboIndiciesArray());
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			//glDrawElementsInstanced(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0, 3);
			// glDrawElementsBaseVertex(GL_TRIANGLES, 3*size/sizeof(Blender::Face), GL_UNSIGNED_INT, 0, );
			glDrawElements(GL_TRIANGLES, size/sizeof(int), GL_UNSIGNED_INT, 0);
		if(useAttr)
			glDisableVertexAttribArray(coord);
		if(useColor)
			glDisableVertexAttribArray(color);
		if(useNorm)
			glDisableVertexAttribArray(normal);
}

Matrix4 getTransformations(Blender::Bone *b) {
	Vector3 dm = Vector3::sub(&(b->tail), &(b->head));
	double dist = Vector3().distance(&dm);
	// dm = dm * (1/dist);
	dm.normalize();
	// std::cout << dist << "\n";
	return Matrix4()
			.lookAt(b->head, b->tail, Vector3(1,0,0))
			// .Scale(dist)
			// .Rotate(1.0f, Vector3(acos(dm.x), acos(dm.y), acos(dm.z)))
			// .Scale(Vector3(1,1,dist))
			.Translate(b->head)
	;
}

std::ostream &operator << (std::ostream &out, Vector3 v);
void GLArmature::render() {
	if((int)prog == -1 || (int)vbo_vertices == -1 || (int)ibo_indicies == -1)
		return;

	Matrix4 model = Matrix4().Translate(location).Rotate(1.0f, rotation).Scale(scale);//
	// if(getParent()) {
		// std::cout << getParent()->location << "\n";
		// model = Matrix4().Translate(getParent()->location);//.Rotate(1.0f, Object::parent->rotation).Scale(Object::parent->scale);
	// }
	glUseProgram(0);
	// std::cout << listBones.size() << " ";
		glColor3f(1,1,0);

	// Reference plane here...
	// glBegin(GL_TRIANGLES);
	// 	glVertex3f(-3, -3, 0);
	// 	glVertex3f(-3, 3, 0);
	// 	glVertex3f(3, 3, 0);

	// 	glVertex3f(3, 3, 0);
	// 	glVertex3f(3, -3, 0);
	// 	glVertex3f(-3, -3, 0);
	// glEnd();
	for (std::list<Blender::Bone *>::iterator it = listBones.begin(); it != listBones.end(); ++it) {
		Blender::Bone *bone = *it;
		if(bone == 0) continue;
		bone = bone->parent;

		Vector3 offs(0,0,0);
		while(bone != 0) {
			offs = offs + bone->tail;
			bone = bone->parent;
		}
		bone = *it;
		glColor3f(0,0,0);
		//glMultMatrixf(b->bone_mat.getMatrix4());

		// b->bone_mat = getTransformations(b);
		// Matrix4 currentTransform = b->bone_mat;
		// Blender::Bone *par = b->parent;
		// while(par != 0) {
		// 	currentTransform = Matrix4().Translate(par->tail) * (par->bone_mat * currentTransform);
		// 	par = par->parent;
		// }

		// glUniformMatrix4fv(getUniform("o_transform"), 1, GL_FALSE, (model * currentTransform).getMatrix4());
		// renderOnce();
		glBegin(GL_LINES);
			glVertex3f(  offs.x + bone->tail.x, offs.y + bone->tail.y, offs.z + bone->tail.z );
			glVertex3f(  offs.x + bone->head.x, offs.y + bone->head.y, offs.z + bone->head.z );
		glEnd();
	}
}/*
void GLArmature::render() {
	if((int)prog == -1 || (int)vbo_vertices == -1 || (int)ibo_indicies == -1)
		return;

	Matrix4 model = Matrix4().Translate(location).Rotate(1.0f, rotation).Scale(scale);//
	// if(getParent()) {
		// std::cout << getParent()->location << "\n";
		// model = Matrix4().Translate(getParent()->location);//.Rotate(1.0f, Object::parent->rotation).Scale(Object::parent->scale);
	// }
	glUseProgram(0);
	for (std::vector<Blender::Bone>::iterator it = bones.begin(); it != bones.end(); ++it) {
		Blender::Bone &parent = *it;
		std::vector<Blender::Bone *> children(parent.children.size(), 0);
		// parent.bone_mat = getTransformations(&parent);
		for (int i = 0; i < parent.children.size(); i++) 
			children[i] = &(parent.children[i]);
			glColor3f(0,0,0);

		while(!children.empty()) {
			Blender::Bone *b = children.back();
			children.pop_back();
			if(b == 0) continue;
			Blender::Bone *par = b->parent;
			Vector3 offs(0,0,0);
			while(par != 0) {
				offs = offs + par->tail;
				par = par->parent;
			}
			//glMultMatrixf(b->bone_mat.getMatrix4());
			glBegin(GL_LINES);
				glVertex3f(  offs.x + b->tail.x, offs.y + b->tail.y, offs.z + b->tail.z );
				glVertex3f(  offs.x + b->head.x, offs.y + b->head.y, offs.z + b->head.z );
			glEnd();
			// b->bone_mat = getTransformations(b);
			// Matrix4 currentTransform = b->bone_mat;
			// Blender::Bone *par = b->parent;
			// while(par != 0) {
			// 	currentTransform = Matrix4().Translate(par->tail) * (par->bone_mat * currentTransform);
			// 	par = par->parent;
			// }

			// glUniformMatrix4fv(getUniform("o_transform"), 1, GL_FALSE, (model * currentTransform).getMatrix4());
			// renderOnce();


			for (int i = 0; i < b->children.size(); i++) 
				children.push_back(&(b->children[i]));
		}
		// glUniformMatrix4fv(getUniform("o_transform"), 1, GL_FALSE, (model * parent.bone_mat).getMatrix4());
		// renderOnce();

		glBegin(GL_LINES);
			glVertex3f(  parent.tail.x, parent.tail.y, parent.tail.z );
			glVertex3f(  parent.head.x, parent.head.y, parent.head.z );
		glEnd();
	}
}*/

GLuint GLArmature::getVboVerticesArray() const {
	return vbo_vertices;
}
GLuint GLArmature::getVboTextureArray() const {
	return vbo_vertices;
}
GLuint GLArmature::getIboIndiciesArray() const {
	return ibo_indicies;
}
bool GLArmature::isModel() {
	return _allokatedMem;
}
void GLArmature::addAttribute(Attribute a) {
	attributes.push_back(a);
}
Attribute GLArmature::getAttribute(GLuint a) const {
	if(a < attributes.size())
		return attributes[a];
	return Attribute();
}
Attribute GLArmature::getAttribute(std::string name) const {
	for (int i = attributes.size(); i > 0; i--) {
		if(attributes[i-1].name == name)
			return attributes[i-1];
	}
	return Attribute();
}
Uniform GLArmature::getUniform(GLuint u) const {
	if(u < uniforms.size())
		return uniforms[u];
	return Uniform();
}
Uniform GLArmature::getUniform(std::string name) const {
	for (int i = uniforms.size(); i > 0; i--) {
		if(uniforms[i-1].name == name)
			return uniforms[i-1];
	}
	return Uniform();
}
void GLArmature::addUniform(Uniform u) {
	uniforms.push_back(u);
}