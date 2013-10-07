#ifndef GLBLENDERWORLD
#define GLBLENDERWORLD
#include <Blender.h>
#include <utils/GLobject.h>
#include <AnimData.h>
#include <exception>

class GLBlender : public GLobject {
	Blender::AnimData adr;
public:
	GLBlender(){}
	int loadMesh(Blender::Mesh *mesh){
		if(mesh == 0) return 0;

		position = mesh->location;
		rotation = mesh->rotation;
		scale = mesh->scale;
		adr = mesh->adr;
		for (int i = 0; i < mesh->verts.size(); i++) {
			Vertex v;
			v.position = mesh->verts[i].position;
			v.worldPosition = mesh->verts[i].position;
			v.localPosition = mesh->verts[i].position;
			v.normal = mesh->verts[i].normal;

			// if(mesh->materials.size() == 0){
			// 	return 0;
			// }
			
			// std::cout << (int)mesh->verts[i].mat_nr << "\n";
			// Blender::Material *mat = mesh->materials.at(mesh->verts[i].mat_nr);
			// v.color[0] = mat->r;
			// v.color[1] = mat->g;
			// v.color[2] = mat->b;
			// v.color[3] = 1.0f;
			vertices.push_back(v);
		}
		for (int i = 0; i < mesh->faces.size(); i++){
			Blender::Face face = mesh->faces.at(i);
			this->faces.push_back(face.v1);
			this->faces.push_back(face.v2);
			this->faces.push_back(face.v3);

			if(mesh->materials.size() == 0){
				return 0;
			}
			Blender::Material *mat = mesh->materials[mesh->faces.at(i).mat_nr];
			if(mat == 0) {
				std::cout << "no material on mesh: " << mesh->name << "\n";
				return 0;
			}

			vertices[face.v1].color[0] = mat->r;
			vertices[face.v1].color[1] = mat->g;
			vertices[face.v1].color[2] = mat->b;
			vertices[face.v1].color[3] = mat->ref;
			
			vertices[face.v2].color[0] = mat->r;
			vertices[face.v2].color[1] = mat->g;
			vertices[face.v2].color[2] = mat->b;
			vertices[face.v2].color[3] = mat->ref;
			
			vertices[face.v3].color[0] = mat->r;
			vertices[face.v3].color[1] = mat->g;
			vertices[face.v3].color[2] = mat->b;
			vertices[face.v3].color[3] = mat->ref;
			
		}
		// for (int i = 0; i < mesh->dverts.size(); i++) {
		// 	if(mesh->dverts[i].totweights != 0 && mesh->dverts[i].weights[0].def_nr == 0) {
		// 		vertices[i].color[0] = mesh->dverts[i].weights[0].weight;
		// 		vertices[i].color[1] = mesh->dverts[i].weights[0].weight;
		// 		vertices[i].color[2] = mesh->dverts[i].weights[0].weight;
		// 		vertices[i].color[3] = 1.0f;//mesh->dverts[i].weights[0].weight;
		// 	}
		// }
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
	int getFrameCount(){
		return adr.getFrameCount();
	}
	bool setFrame(int frame) {
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
};

class InitWorld : public Blender::BlenderCallback {
	std::vector<GLBlender *> glblend;
public:
	InitWorld(){}
	~InitWorld(){
		for (int i = 0; i < glblend.size(); i++) {
			delete glblend[i];
		}
	}
	void objectLoaded(Blender::Object *obj){
		if(obj == 0)
			return;
		if(obj->getType() == Blender::Mesh::ID){
			Blender::Mesh *mesh = static_cast<Blender::Mesh *>(obj);

			GLBlender *glb = new GLBlender();
			glb->loadMesh(mesh);
			glblend.push_back(glb);
		}
	}
	Blender::DCollection CollectionOfData(){
		return Blender::MESH;
	}

	size_t getObjectCount(){
		return glblend.size();
	}
	GLBlender *getMesh(size_t i){
		return glblend[i];
	}
};

#endif