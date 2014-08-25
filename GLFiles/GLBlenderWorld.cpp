#include "GLBlenderWorld.h"

int GLBlender::loadMesh(Blender::Mesh *mesh){
	if(mesh == 0) return 0;
	vbo_vertices = -1;
	ibo_indicies = -1; 
	vbo_textureCoordinates = -1; 
	prog = -1;
	if(_allokatedMem) {
		vertices.clear();
		faces.clear();
		_allokatedMem = false;
	}
	linkedShaders = false;

	std::cout << "A mesh...\n" << scale.z * scale.y * scale.x << "\n";
	return 0;
	position = mesh->location;
	rotation = mesh->rotation;
	scale.x = mesh->scale.x; scale.y = mesh->scale.y; scale.z = mesh->scale.z; 
	adr = mesh->adr;
	// name = mesh->name;
	for (int i = 0; i < mesh->verts.size(); i++) {
		Vertex v;
		v.position = mesh->verts[i].position;
		v.worldPosition = mesh->verts[i].position;
		v.localPosition = mesh->verts[i].position;
		v.normal = mesh->verts[i].normal;
		// std::cout << (unsigned int)-1 << "," << prog << "\n";
		vertices.push_back(v);
	}
	// std::cout << mesh->name << "hjuidasdsa\n";
	// return 1;
	for (int i = 0; i < mesh->faces.size(); i++){
		Blender::Face face = mesh->faces[i];
		unsigned int vs = vertices.size();

		if(vs > face.v1) 
			this->faces.push_back(face.v1);
		if(vs > face.v2)
			this->faces.push_back(face.v2);
		if(vs > face.v3) 
			this->faces.push_back(face.v3);
		if(mesh->materials.size() == 0 || mesh->materials.size() <= mesh->faces[i].mat_nr){
			return 0;
		}
		Blender::Material *mat = mesh->materials[mesh->faces.at(i).mat_nr];
		if(mat == 0) {
			std::cout << "no material on mesh: " << mesh->name << "\n";
			return 0;
		}
		if(vs > face.v1) {
			vertices[face.v1].color[0] = mat->r;
			vertices[face.v1].color[1] = mat->g;
			vertices[face.v1].color[2] = mat->b;
			vertices[face.v1].color[3] = mat->ref;
		}
		if(vs > face.v2) {
			vertices[face.v2].color[0] = mat->r;
			vertices[face.v2].color[1] = mat->g;
			vertices[face.v2].color[2] = mat->b;
			vertices[face.v2].color[3] = mat->ref;
		}
		if(vs > face.v3) {	
			vertices[face.v3].color[0] = mat->r;
			vertices[face.v3].color[1] = mat->g;
			vertices[face.v3].color[2] = mat->b;
			vertices[face.v3].color[3] = mat->ref;
		}
	}
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
		return status; // abort
	return 1;
}

bool GLBlender::setFrame(int frame) {
	if(frame < 1) return false;
	int cc = adr.getCurveCount();
	for (int i = 0; i < cc; i++){
		Blender::Curve cu = adr.getCurve(i);

		int totvert = cu.getTotVert();
		for (int j = 0; j < totvert; j++) {
			Blender::BezTriple bezt = cu.keyframes[j];
			if(bezt.cursor.x == frame){
				if(cu.rna_path == "location")
					position[cu.data_index] = bezt.cursor.y;
				if(cu.rna_path == "rotation_euler" || cu.rna_path == "rotation")
					rotation[cu.data_index] = bezt.cursor.y;
				if(cu.rna_path == "scale")
					scale[cu.data_index] = bezt.cursor.y;
				break;
			}
			if(bezt.cursor.x > frame){
				if(j == 0) return false;
				Blender::BezTriple bezt2 = cu.keyframes[j-1];
				float nom = bezt.cursor.x-bezt2.cursor.x;
				if(nom == 0)
					return false;
				float k = (bezt.cursor.y-bezt2.cursor.y)/nom;
				float m = bezt.cursor.y - k * bezt.cursor.x;

				if(cu.rna_path == "location")
					position[cu.data_index] = k * frame + m;
				if(cu.rna_path == "rotation_euler" || cu.rna_path == "rotation")
					rotation[cu.data_index] = k * frame + m;
				if(cu.rna_path == "scale")
					scale[cu.data_index] = k * frame + m;
				break;
			}
		}
	}
	return true;
}