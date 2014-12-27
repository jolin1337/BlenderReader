#include "GLMesh.h"

// char ** GLMesh::v_shader = {
// 	"Something"
// };
// char ** GLMesh::f_shader = {
// 	"Something"
// };

GLMesh::GLMesh():vbo_vertices(-1), ibo_indicies(-1), vbo_textureCoordinates(-1), 
				 prog(-1), _allokatedMem(false), linkedShaders(false) {
}

GLMesh::~GLMesh() {
	if((int)prog != -1)
		glDeleteProgram(prog);
	if((int)vbo_vertices != -1)
		glDeleteBuffers(1, &vbo_vertices);
	if((int)ibo_indicies != -1)
		glDeleteBuffers(1, &ibo_indicies);

	if((int)vbo_textureCoordinates != -1)
		glDeleteBuffers(1, &vbo_textureCoordinates);
}

GLMesh::GLMesh(const Blender::Mesh &mesh):Blender::Mesh(mesh), 
				 vbo_vertices(-1), ibo_indicies(-1), vbo_textureCoordinates(-1), 
				 prog(-1), _allokatedMem(false), linkedShaders(false) {
	initOPGL();
}
GLMesh::GLMesh(const GLMesh &mesh):Blender::Mesh(mesh), vbo_vertices(-1), ibo_indicies(-1), vbo_textureCoordinates(-1), 
				 prog(-1), _allokatedMem(false), linkedShaders(false) {
	throw "Not implemented";
	initOPGL();
}

GLMesh &GLMesh::operator=(const Blender::Mesh &mesh) {
	throw "Not implemented";
	Blender::Mesh::operator=(mesh);
	initOPGL();
}

GLMesh &GLMesh::operator=(const GLMesh &mesh) {
	throw "Not implemented";
	Blender::Mesh::operator=(mesh);
	initOPGL();
}


void GLMesh::initOPGL() {
	if(size() == 0 || f_size() == 0) return;
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

void GLMesh::setupBuffers() {
	if(size() == 0 || f_size() == 0) return;
	glGenBuffers(1, &ibo_indicies);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_indicies);
	
	int *indicies_buffer = new int[f_size()*3];
	for (int i = 0; i < f_size()*3; i+=3) {
		indicies_buffer[i] = getFaces()[i/3].v1;
		indicies_buffer[i+1] = getFaces()[i/3].v2;
		indicies_buffer[i+2] = getFaces()[i/3].v3;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, f_size()*3*sizeof(int), indicies_buffer, GL_STATIC_DRAW);

	delete[] indicies_buffer;

	Vertex_cp * vs = getVertices();
	unsigned int vsize = size();
	for(int i=0;i<f_size();i++) {
		Blender::Material mat;
		Blender::Face f = faces[i];
		if(f.mat_nr < 0 || f.mat_nr >= materials.size()) {
			mat = *materials[0];
			// std::cout << (int)f.mat_nr << "," << materials.size() << "\n";
		}
		else
			 mat = *materials[f.mat_nr];
		std::stringstream color("Color\n");
		color << "Red: " << mat.r << ", Green: " << mat.g << ", Blue: " << mat.b;
		// BlenderGlobals::Log(color.str());
		if(f.v1 < vsize && f.v1 > -1)
			vs[f.v1].mat = mat;
		if(f.v2 < vsize && f.v2 > -1)
			vs[f.v2].mat = mat;
		if(f.v3 < vsize && f.v3 > -1)
			vs[f.v3].mat = mat;
	}
	
	glGenBuffers(1, &(vbo_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, vsize*sizeof(Vertex_cp), vs, GL_DYNAMIC_DRAW);
}


// int GLMesh::getFrameCount() {
// 	return adr.getFrameCount();
// }

// bool GLMesh::setFrame(int frame) {
// 	if(frame < 1) return false;
// 	int cc = adr.getCurveCount();
// 	for (int i = 0; i < cc; i++){
// 		Blender::Curve cu = adr.getCurve(i);

// 		int totvert = cu.getTotVert();
// 		for (int j = 0; j < totvert; j++) {
// 			Blender::BezTriple bezt = cu.keyframes[j];
// 			if(bezt.cursor.x == frame){
// 				if(cu.rna_path == "location")
// 					location[cu.data_index] = bezt.cursor.y;
// 				if(cu.rna_path == "rotation_euler" || cu.rna_path == "rotation")
// 					rotation[cu.data_index] = bezt.cursor.y;
// 				if(cu.rna_path == "scale")
// 					scale[cu.data_index] = bezt.cursor.y;
// 				break;
// 			}
// 			else if(bezt.cursor.x > frame){
// 				if(j == 0) return false;
// 				Blender::BezTriple bezt2 = cu.keyframes[j-1];
// 				float nom = bezt.cursor.x-bezt2.cursor.x;
// 				if(nom == 0)
// 					return false;
// 				float k = (bezt.cursor.y-bezt2.cursor.y)/nom;
// 				float m = bezt.cursor.y - k * bezt.cursor.x;

// 				if(cu.rna_path == "location")
// 					location[cu.data_index] = k * frame + m;
// 				if(cu.rna_path == "rotation_euler" || cu.rna_path == "rotation")
// 					rotation[cu.data_index] = k * frame + m;
// 				if(cu.rna_path == "scale")
// 					scale[cu.data_index] = k * frame + m;
// 				break;
// 			}
// 			else {
// 			}
// 		}
// 	}
// 	return true;
// }


GLuint GLMesh::getProgram() const {
	return prog;
}
int GLMesh::setupUniforms() {
	if(!linkedShaders)return 0;
	int res = 1;
	for(Uniforms::iterator it = uniforms.begin(); it != uniforms.end(); it++)
		if((*it).setUp(prog) == 0)
			res = 0;
	return res;
}
int GLMesh::setupAttributes() {
	if(!linkedShaders)return 0;
	int res = 1;
	for(Attributes::iterator it = attributes.begin(); it != attributes.end(); it++)
		if((*it).setUp(prog) == 0)
			res = 0;
	return res;
}

int GLMesh::loadShaders(std::string fileName_v, std::string fileName_f) {
	Uniform otr = getUniform("o_transform");
	if(!otr.isDefined())
		addUniform(Uniform("o_transform"));

	GLint link_ok = GL_FALSE;
	GLuint vs, fs;
	if ((vs = create_shader(fileName_v.c_str(), GL_VERTEX_SHADER))   == 0) return 0;
	if ((fs = create_shader(fileName_f.c_str(), GL_FRAGMENT_SHADER)) == 0) return 0;

	prog = glCreateProgram();
	std::cout << prog << "\n";

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

std::ostream &operator << (std::ostream &out, Vector3 v);
void GLMesh::render() {
	if((int)prog == -1 || (int)vbo_vertices == -1 || (int)ibo_indicies == -1)
		return;
	Matrix4 model = Matrix4().Translate(location).Rotate(1.0f, rotation).Scale(scale);//
	// if(getParent()) {
		// std::cout << getParent()->location << "\n";
		// model = Matrix4().Translate(getParent()->location);//.Rotate(1.0f, Object::parent->rotation).Scale(Object::parent->scale);
	// }
	
	
	glUniformMatrix4fv(getUniform("o_transform"), 1, GL_FALSE, model.getMatrix4());

	Attribute coord = getAttribute("position"),
			  color = getAttribute("color"),
			  normal = getAttribute("normal");
	bool useAttr = coord.isDefined(), 
		useColor = color.isDefined() && color.name != coord.name,
		useNorm = normal.isDefined() && normal.name != color.name && normal.name != coord.name;

		// glEnableClientState(GL_COLOR_ARRAY);

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
					(void*)offsetof(Vertex_cp,mat)
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


	// glDisableClientState(GL_COLOR_ARRAY);
}
void GLMesh::updateElement() {
	updateElement(0, size());
}
void GLMesh::updateElement(GLuint from, GLuint size) {
	if(size+from > verts.size()) 
		return;

	glBindBuffer(GL_ARRAY_BUFFER, getVboVerticesArray());
	glBufferSubData(GL_ARRAY_BUFFER, from, size*sizeof(Vertex_cp), getVertices());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void GLMesh::updateElement(GLuint element) {
	updateElement(element, 1);
}
GLuint GLMesh::getVboVerticesArray() const {
	return vbo_vertices;
}
GLuint GLMesh::getVboTextureArray() const {
	return vbo_vertices;
}
GLuint GLMesh::getIboIndiciesArray() const {
	return ibo_indicies;
}
bool GLMesh::isModel() {
	return _allokatedMem;
}
void GLMesh::addAttribute(Attribute a) {
	attributes.push_back(a);
}
Attribute GLMesh::getAttribute(GLuint a) const {
	if(a < attributes.size())
		return attributes[a];
	return Attribute();
}
Attribute GLMesh::getAttribute(std::string name) const {
	for (int i = attributes.size(); i > 0; i--) {
		if(attributes[i-1].name == name)
			return attributes[i-1];
	}
	return Attribute();
}
Uniform GLMesh::getUniform(GLuint u) const {
	if(u < uniforms.size())
		return uniforms[u];
	return Uniform();
}
Uniform GLMesh::getUniform(std::string name) const {
	for (int i = uniforms.size(); i > 0; i--) {
		if(uniforms[i-1].name == name)
			return uniforms[i-1];
	}
	return Uniform();
}
void GLMesh::addUniform(Uniform u) {
	uniforms.push_back(u);
}